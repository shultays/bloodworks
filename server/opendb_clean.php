<?php
$servername = "";
$username_db = "";
$password_db = "";
$dbname = "";
$reportPass = "";

$link = mysqli_init();

// Create connection
$conn = mysqli_real_connect($link, $servername, $username_db, $password_db, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
asd
?>