<?php
$success = "error";
if(isset($_POST['username']) and isset($_POST['password']))
{
	include 'hasher.php';

	if(validUsername($_POST['username']) and validPassword($_POST['password'])) 
	{ 
		$success = "true";
		
		include 'opendb.php';

		$query = "SELECT password FROM users  WHERE username = '".$_POST['username']."' LIMIT 1";
		$result = mysqli_query($link, $query);
		include 'closedb.php';
		if (mysqli_num_rows($result) == 0) 
		{
			$success = "username does not exist";	
		}
		else
		{
			list($password) = mysqli_fetch_array($result);
			if ($password === hashText($_POST['password']))
			{
				$success = "true";
			}
			else
			{
				$success = "wrong password";
			}
		}
	}
	else
	{
		$success = "invalid characters";	
	}
}
echo $success;
?>