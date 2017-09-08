<?php
$success = "error";

include_once 'hasher.php';

$isfeedback = false;
$max  = 10;
if($_SERVER['HTTP_REFERER'])
{
    $isfeedback = true;
    $max  = 0;
}

if(isset($_POST['report']))
{
    $ip = hashIP($_SERVER['REMOTE_ADDR']);
    include 'opendb.php';
    include 'check_spam.php';

    $report = $link->real_escape_string($_POST['report']);
    $message = $link->real_escape_string($_POST['message']);
    
    if (checkSpam($link, 60, $max, "report") == false) {
        echo "Please wait before sending new reports.";
        include 'closedb.php';
        exit();
    }
    $query = "INSERT INTO reports (ip, message, report, feedback) VALUES ('$ip', '$message', '$report', '$isfeedback')";
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