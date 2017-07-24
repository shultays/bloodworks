<?php

include 'opendb.php';
$query = "SELECT name, description, version, creator FROM upload LIMIT 100";
$result = mysqli_query($link, $query);
include 'closedb.php';
while($row = $result->fetch_assoc()) {
	echo $row["name"] . "<br>";
	echo $row["description"] . "<br>";
	echo $row["version"] . "<br>";
	echo $row["creator"] . "<br>";
}

?>