{
<?php
$start = 0;
$limit = 100;
include_once 'opendb.php';

if (isset($_GET["start"]))
{
	$start = $link->real_escape_string($_GET["start"]);
}
if (isset($_GET["limit"]))
{
	$limit = $link->real_escape_string($_GET["limit"]);
}

$query = "SELECT id, name, description, version, creator, folder FROM upload LIMIT ".$limit." OFFSET ".$start;
$result = mysqli_query($link, $query);
include_once 'closedb.php';
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
		"id" : '.$row["id"].',
		"name" : "'.$row["name"].'",
		"description" : "'.$row["description"].'",
		"version" : "'.$row["version"].'",
		"creator" : "'.$row["creator"].'",
		"folder" : "'.$row["folder"].'"
	}';
	$first = false;
}
echo ']';
?>
}