<?php
$success = "error";
if(isset($_POST['username']) and isset($_POST['password']))
{
	include_once 'hasher.php';

	$username = $_POST['username'];
	$username = addslashes($username);
	$password = $_POST['password'];
	$password = addslashes($password);
	
	if(validUsername($username) and validPassword($password)) 
	{
		include_once 'opendb.php';
        include_once 'check_spam.php';
        if (checkSpam($link, 30, 0, "register") == false) {
            echo "you can only register once 30 secs";
            include_once 'closedb.php';
            exit();
        }
		$query = "SELECT password FROM users WHERE username = '$username' LIMIT 1";
		$result = mysqli_query($link, $query);
		if (mysqli_num_rows($result) == 0) 
		{
			$hashedPassword = hashText($password);
			$query = "INSERT INTO users (username, password) VALUES ('$username', '$hashedPassword')";
			if (mysqli_query($link, $query)) 
			{
				$success = "true";
		
			} else {
				$success = "error createing user";
			}			
		}
		else
		{
			$success = "username already exists";	
		}
		include_once 'closedb.php';
	}
	else
	{
		$success = "invalid password or username";	
	}
}
echo $success;
?>