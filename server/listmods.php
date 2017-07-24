{
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
$query = "SELECT name, description, version, creator FROM upload LIMIT ".$limit." OFFSET ".$start;
$result = mysqli_query($link, $query);
include 'closedb.php';
echo '"count" : ' . mysqli_num_rows ($result) . ',';
echo '"mods" : [';
$first = true;
while($row = $result->fetch_assoc()) 
{
	if ($first == false)
	{
		echo "\n,";
	}
	echo '{
		"name" : "'.$row["name"].'",
		"description" : "'.$row["description"].'",
		"version" : "'.$row["version"].'",
		"creator" : "'.$row["creator"].'"
	}';
	$first = false;
}
echo ']';
?>
}