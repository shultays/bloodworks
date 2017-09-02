<?php
$success = "error";
if(isset($_POST['report']))
{
    $report = addslashes($_POST['report']);
    $message = addslashes($_POST['message']);
    $ip = $_SERVER['REMOTE_ADDR'];
    include 'opendb.php';
    $query = "INSERT INTO reports (ip, message, report) VALUES ('$ip', '$message', '$report')";
    if (mysqli_query($link, $query)) 
    {
        $success = "Report has been received. Thanks!";

    } else {
        $success = "error sending report";
    }	
    include 'closedb.php';
}
echo $success;
?>