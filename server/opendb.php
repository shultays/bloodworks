<?php
$servername = "mysql.enginmercan.com";
$username_db = "bloodworks_db";
$password_db = "k2a0631k2NTa!!-";
$dbname = "bloodworks_files";

$link = mysqli_init();

// Create connection
$conn = mysqli_real_connect($link, $servername, $username_db, $password_db, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>