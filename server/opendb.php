<?php
$servername = "mysql.enginmercan.com";
$username = "bloodworks_db";
$password = "k2a0631k2NTa!!-";
$dbname = "bloodworks_files";

$link = mysqli_init();

// Create connection
$conn = mysqli_real_connect($link, $servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>