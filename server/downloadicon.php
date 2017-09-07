<?php
ob_start();
if(isset($_GET['id']))
{
	// if id is set then get the file with the id from database

	include_once 'opendb.php';

	$id    = $_GET['id'];
	$query = "SELECT icon " .
			 "FROM upload WHERE id = '$id'";

	$result = mysqli_query($link, $query) or die('Error, query failed');
	list($icon) = mysqli_fetch_array($result);

	header("Content-Disposition: attachment; filename=icon.png");
	ob_clean();
	flush();
	echo $icon;

	include_once 'closedb.php';
	exit;
}
?>
