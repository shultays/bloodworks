<?php
function endsWith($haystack, $needle)
{
    $length = strlen($needle);
    if ($length == 0) {
        return true;
    }

    return (substr($haystack, -$length) === $needle);
}

function isValidFile($name) {
	return endsWith($name, ".lua") or endsWith($name, ".png") or endsWith($name, ".json") or endsWith($name, ".ogg");
} 

$success = "error";
$validUser = false;

include 'opendb.php';
$userid = "-1";

if(isset($_POST['username']) and isset($_POST['password']))
{
	$username = $_POST['username'];
	$username = addslashes($username);
	$password = $_POST['password'];
	$password = addslashes($password);
	
	include 'hasher.php';

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
if (isset($_POST['upload']) and $_FILES['userfile']['size'] > 0)
{
	$validUpload = true;
}
else
{
	$success = "upload error";
}

if($validUser and $validUpload)
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
			$fileName = addslashes($fileName);
			$fileType = addslashes($fileType);
			$content = addslashes($content);
			$type = addslashes($type);

			$query = "INSERT INTO upload (name, size, type, content, userid ) ".
			"VALUES ('$fileName', '$fileSize', '$fileType', '$content', '$userid')";

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
			$success = "invalid files";
		}
	}
}

echo $success;
include 'closedb.php';
?>
