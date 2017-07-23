<?php
function hashText($text)
{
	$options = [
		'cost' => 11,
		'salt' => 'salti_sugar431292CAKE!!',
	];
	return password_hash($text, PASSWORD_BCRYPT, $options);
}
function validUsername($text)
{    
	$regex = "[a-zA-Z0-9 \-\+\._=\@\!\?,;~#\(\)\[\]\{\}\*\/]+";
	return preg_match("/^$regex$/i", $text);
}
function validPassword($text)
{    
	return validUsername($text);
}
?>