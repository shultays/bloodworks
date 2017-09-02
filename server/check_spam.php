<?php

function checkSpam($link, $timelimit, $allowed, $column)
{
    $ret = false;
    $ip = $_SERVER['REMOTE_ADDR'];
    
    $column_last_request = $column . "_last_request";
    $column_total_request = $column . "_total_request";
    
    $query = "SELECT $column_last_request, $column_total_request FROM spam WHERE ip = '$ip' LIMIT 1";
    $result = mysqli_query($link, $query);

    $t = time();
    if (mysqli_num_rows($result) == 0) 
    {
        $query = "INSERT INTO spam (ip, $column_last_request, $column_total_request) VALUES ('$ip', $t , 0)";
        mysqli_query($link, $query);
        $ret = true;
    }
    else
    {
        $row = $result->fetch_assoc();
        $last_request =  $row[$column_last_request];
        $total_request =  $row[$column_total_request];
        
        //echo ($t - $last_request) . " " . $total_request . "    " . $allowed . "<br>";
        $hasTime = ($t - $last_request) > $timelimit;
        $ret = $hasTime || ($total_request < $allowed);
        if ($hasTime){
            $total_request = 0;
        } else if ($ret) {
            $total_request++;
        }
        if ($ret) {
            $query = "UPDATE spam SET $column_last_request = $t, $column_total_request = $total_request WHERE ip = '$ip'";
            $result = mysqli_query($link, $query);
        }
    }
    
    return $ret;
}


?>