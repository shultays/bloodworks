<?php
ob_start();
if(isset($_GET['id']))
{
	// if id is set then get the file with the id from database

	include_once 'opendb.php';

	$id    = $_GET['id'];
	$query = "SELECT name, type, size, content " .
			 "FROM upload WHERE id = '$id'";

	$result = mysqli_query($link, $query) or die('Error, query failed');
	list($name, $type, $size, $content) = mysqli_fetch_array($result);

	header("Content-length: $size");
	header("Content-type: $type");
	header("Content-Disposition: attachment; filename=$name");
	ob_clean();
	flush();
	echo $content;

	include_once 'closedb.php';
	exit;
}
?>
