#!/usr/bin/php
<?php
define('DEBUG',$argc>1);
require('./puzzle.inc');
echo totp(trim(file_get_contents(".lob-secret.txt")))."\n";
$arg = DEBUG?"1":"";
echo `./puzzle $arg`;
