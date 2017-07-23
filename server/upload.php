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

if(isset($_POST['upload']) and $_FILES['userfile']['size'] > 0)
{
	$fileName = $_FILES['userfile']['name'];
	$tmpName  = $_FILES['userfile']['tmp_name'];
	$fileSize = $_FILES['userfile']['size'];
	$fileType = $_FILES['userfile']['type'];

	$fp      = fopen($tmpName, 'r');
	$content = fread($fp, filesize($tmpName));
	fclose($fp);

	$headerSize = unpack("i", $content)[1];

	$headerSize -= 4;
	$header = substr ( $content , 4, $headerSize);

	$fileNum = unpack("i", $header)[1];
	echo $fileNum . "\n" ;
	$headerSize -= 4;
	$header = substr ( $header , 4, $headerSize);

	$allValid = true;

	for( $i = 0; $i<$fileNum; $i++ ) 
	{
		$len = unpack("i", $header)[1];
		echo $len . "\n" ;
		$headerSize -= 4;
		$header = substr($header, 4, $headerSize);
		
		$name = substr($header, 0, $len);
		echo $name . "\n" ;
		$headerSize -= $len + 4;
		$header = substr($header, $len + 4, $headerSize);
		
		if (isValidFile($name) == false){
			$allValid = false;
		}
	}

	echo $allValid . "\n";

	if(!get_magic_quotes_gpc())
	{
		$fileName = addslashes($fileName);
	}

	if ($allValid)
	{
		$content = addslashes($content);
		include 'opendb.php';

		$query = "INSERT INTO upload (name, size, type, content ) ".
		"VALUES ('$fileName', '$fileSize', '$fileType', '$content')";

		mysqli_query($link, $query) or die('Error, query failed');
		include 'closedb.php';
		echo "<br>File $fileName uploaded<br>";
	}
}
?>
<html>
<body>
<form method="post" enctype="multipart/form-data">
<table width="350" border="0" cellpadding="1" cellspacing="1" class="box">
<tr>
<td width="246">
<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
<input name="userfile" type="file" id="userfile">
</td>
<td width="80"><input name="upload" type="submit" class="box" id="upload" value=" Upload " ></td>
</tr>
</table>
</form>
</body>
</html>
