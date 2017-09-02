
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
    $query = "SELECT ip, message, report, time FROM reports LIMIT ".$limit." OFFSET ".$start;
    $result = mysqli_query($link, $query);
}
include 'closedb.php';

while($row = $result->fetch_assoc()) 
{
    echo $row["ip"] . "<br>";
    echo $row["time"] . "<br>";
    echo $row["message"] . "<br>";
    echo "<br>report:<br>";
    $report = $row["report"];
    $report = str_replace("\n", "<br>", $report);
    echo $report . "<br>";
    echo "================================================<br><br><br>";
}
?>