
<?php
$start = 0;
$limit = 100;
if (isset($_GET["start"]))
{
	$start = $_GET["start"];
}
if (isset($_GET["limit"]))
{
	$limit = $_GET["limit"];
}

include 'opendb.php';

if ($_GET["pass"] == $reportPass)
{
    $query = "SELECT id, ip, message, report, time FROM reports ORDER BY id DESC LIMIT ".$limit." OFFSET ".$start;
    $result = mysqli_query($link, $query);
}
include 'closedb.php';


while($row = $result->fetch_assoc()) 
{
    echo "id:" . $row["id"] . "<br>";
    echo "ip:" . $row["ip"] . "<br>";
    
    $date = new DateTime($row["time"], new DateTimeZone('CET'));
    echo "time:" . $date->format('Y-m-d H:i:sP') . "<br>";
    
    echo "message:" . $row["message"] . "<br>";
    echo "<br>report:<br>";
    $report = $row["report"];
    $report = str_replace("\n", "<br>", $report);
    echo $report . "<br>";
    echo "================================================<br><br><br>";
}
?>