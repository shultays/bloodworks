<?php
$success = "error";
if(isset($_POST['username']) and isset($_POST['password']))
{
	include_once 'hasher.php';

	if(validUsername($_POST['username']) and validPassword($_POST['password'])) 
	{ 
		$success = "true";
		
		include_once 'opendb.php';

        include_once 'check_spam.php';

        if (checkSpam($link, 10, 20, "login") == false) {
            echo "please dont spam login";
            include_once 'closedb.php';
            exit();
        }
		$query = "SELECT password FROM users  WHERE username = '".$_POST['username']."' LIMIT 1";
		$result = mysqli_query($link, $query);
		include_once 'closedb.php';
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