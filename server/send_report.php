<?php
$success = "error";

include_once 'hasher.php';

if(isset($_POST['report']))
{
    $ip = hashIP($_SERVER['REMOTE_ADDR']);
    include 'opendb.php';
    include 'check_spam.php';

    $report = $link->real_escape_string($_POST['report']);
    $message = $link->real_escape_string($_POST['message']);
    
    if (checkSpam($link, 60, 10, "report") == false) {
        echo "please dont kill my server";
        include 'closedb.php';
        exit();
    }
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