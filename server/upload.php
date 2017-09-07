<?php
function clearForJson($str)
{
	$str = str_replace('"', "", $str);
	$str = str_replace('$', "", $str);
	$str = str_replace('\\', "", $str);
	$str = str_replace('\n', "", $str);
	$str = str_replace('\r', "", $str);
	$str = str_replace('\t', "", $str);
	return $str;
}
function endsWith($haystack, $needle)
{
    $length = strlen($needle);
    if ($length == 0) {
        return true;
    }

    return (substr($haystack, -$length) === $needle);
}

function isValidFile($name) {
	return endsWith($name, ".lua") or endsWith($name, ".png") or endsWith($name, ".json") or endsWith($name, ".ogg") 
    or endsWith($name, ".vs") or endsWith($name, ".ps");
} 

$success = "error";
$validUser = false;

include_once 'opendb.php';
include_once 'hasher.php';
include_once 'check_spam.php';

if (checkSpam($link, 60, 2, "upload") == false) {
    echo "please dont kill my server";
    include_once 'closedb.php';
    exit();
}
$userid = "-1";
$validHeader = false;

$folderName = "";

if(isset($_POST['foldername']))
{
	$folderName = $_POST['foldername'];
	$folderName = preg_replace('/ /i', '_', $folderName);
	$folderName = preg_replace('/[^a-z0-9A-Z_]/i', '', $folderName);
	$folderName = clearForJson(addslashes($folderName));
}
	
if(isset($_POST['name']) and strlen($_POST['name']) > 0 and validUsername($_POST['name']))
{
	$validHeader = true;
	$modname = $_POST['name'];
	$modname = clearForJson(addslashes($modname));
	$description = (isset($_POST['description']) ? $_POST['description'] : "");
	$description = clearForJson(addslashes($description));
	$version = (isset($_POST['version']) ? $_POST['version'] : "0");
	$version = clearForJson(addslashes($version));
	$creator = (isset($_POST['creator']) ? $_POST['creator'] : "");
	$creator = clearForJson(addslashes($creator));
	
	if (isset($_FILES['icon']))
	{
		if ($_FILES['icon']['size'] > 0)
		{
			$tmpName  = $_FILES['icon']['tmp_name'];

			$fp      = fopen($tmpName, 'r');
			$icon = fread($fp, filesize($tmpName));
			$icon = addslashes($icon);
			fclose($fp);
		}
		else
		{
			$validHeader = false;
			$success = "invalid icon";
		}
	}
	else
	{
		$icon = "";
	}
}
else
{
	$success = "invalid mod name";
}

if(isset($_POST['username']) and isset($_POST['password']))
{
	$username = $_POST['username'];
	$username = addslashes($username);
	$password = $_POST['password'];
	$password = addslashes($password);

	if(validUsername($username) and validPassword($password)) 
	{ 
		$query = "SELECT password, id FROM users WHERE username = '$username' LIMIT 1";
		$result = mysqli_query($link, $query);
		if (mysqli_num_rows($result) == 0) 
		{
			$success = "username does not exist";	
		}
		else
		{
			list($realpass, $userid) = mysqli_fetch_array($result);
			if ($realpass === hashText($password))
			{
				$validUser = true;
			}
			else
			{
				$success = "invalid password";
			}
		}
	}
}

$validUpload = false;
if (isset($_POST['upload']) and isset($_FILES['userfile']) and  $_FILES['userfile']['size'] > 0)
{
	$validUpload = true;
}
else
{
	$success = "upload error";
}


$modid = -1;
$validFile = true;
$query = "SELECT userid, id FROM upload WHERE name = '$modname' LIMIT 1";
$result = mysqli_query($link, $query);
if ($result)
{
    while($row = $result->fetch_assoc()) 
    {
        $modid = $row["id"];
        if ($row["userid"] != $userid)
        {
            $success = "mod is already uploaded for another user";
            $validFile = false;
        }
    }
}

if($validHeader and $validUser and $validUpload and $validFile)
{
	$fileName = $_FILES['userfile']['name'];
	$tmpName  = $_FILES['userfile']['tmp_name'];
	$fileSize = $_FILES['userfile']['size'];
	$fileType = $_FILES['userfile']['type'];

	$fp      = fopen($tmpName, 'r');
	$content = fread($fp, filesize($tmpName));
	fclose($fp);

	$sizes = unpack("ifileSize/iheaderSize", $content);
	
	$fileSizeInHeader = $sizes["fileSize"];
	
	if ($fileSizeInHeader !== $_FILES['userfile']['size'])
	{
		$success = "upload error";
	}
	else
	{
		$headerSize = $sizes["headerSize"];
		// echo "fileSizeInHeader " . $fileSizeInHeader . "\n" ;
		// echo "headerSize " . $headerSize . "\n" ;
		// echo "_FILESS " . $_FILES['userfile']['size'] . "\n" ;

		$header = substr ($content , 8, $headerSize - 8);

		$fileNum = unpack("i", $header)[1];
		// echo $fileNum . "\n" ;
		$header = substr ($header , 4);

		$allValid = true;

		// echo $header . "\n\n";
		for( $i = 0; $i<$fileNum; $i++ ) 
		{
			$len = unpack("i", $header)[1];
			// echo $len . "\n" ;
			$header = substr($header, 4);
			
			$name = substr($header, 0, $len);
			// echo $name . "\n" ;
			$header = substr($header, $len + 4);
			
			if (isValidFile($name) == false){
				$allValid = false;
				// echo $name;
			}
		}

		// echo $allValid . "\n";
		
		if(!get_magic_quotes_gpc())
		{
			$fileName = addslashes($fileName);
		}

		if ($allValid)
		{
            
            if ($modid != -1)
            {
                $count = 0;
            } 
            else 
            {
                $query = "SELECT COUNT(*) FROM upload";
                $result = mysqli_query($link, $query);
                $count = mysqli_fetch_array($result)[0];
            }
			if ($count < 100)
			{
                $fileName = addslashes($fileName);
                $fileType = addslashes($fileType);
                $content = addslashes($content);
                $type = addslashes($type);
        
                if ($modid == -1)
                {
                    $query = "INSERT INTO upload (name, description, version, creator, icon, folder, size, type, content, userid ) ".
                    "VALUES ('$modname', '$description', '$version', '$creator', '$icon', '$folderName', '$fileSize', '$fileType', '$content', '$userid')";
                }
                else
                {
                    $query = "UPDATE upload SET name='$modname', description = '$description', version = '$version', creator = '$creator', icon = '$icon', folder = '$folderName', size = '$fileSize', type = '$fileType', content = '$content', userid = '$userid' WHERE id = '$modid' ";
                }
                if (mysqli_query($link, $query))
                {
                    $success = "true";
                }
                else
                {
                    $success = "sql error";
                }
            }
            else
            {
                $success = "too many mods, come again later!";
            }
		}
		else
		{
			$success = "invalid files";
		}
	}
}

echo $success;
include_once 'closedb.php';
?>
