<?php

!defined('DEBUG') && define('DEBUG',0);

function hotp($buf1){
  if(!count($buf1))
    return false;
  $xbuf = array(0x7f,0xff,0x7f,0xff,0xff);
	$c = 0;
	reset($buf1);
	do{
		$buf[$c++] = current($buf1);
	}while(next($buf1));
  return puzzle(count($buf)-1,4,$xbuf,6,$buf,1,1);  
}

function puzzle(){
  list($off,$cbyte,$xbuf,$pow,$buf1,$t,$p) = func_get_args();
if(DEBUG) echo "BAR $off\n"; 
  $offset = (int)($t?(((int)$buf1[(int)$off]&0xf0)>>4)&0xf:((int)$buf1[(int)$off] &0xf) % count($buf1));
if(DEBUG) echo "FOO $offset\n";
  $j=8*($cbyte-1);
  $int = 0;
if(DEBUG) echo implode(',',array_keys($buf1))."\n";
  for($i =0;$i<$cbyte;$i++,$j-=8){
if(DEBUG) echo "$offset $i $j\n";
    $next = $p?(((int)$buf1[$offset+$i]&0xff)^((int)$xbuf[$i]&0xff))<<(int)$j
              :((int)$buf1[$offset+$i]&(int)$xbuf[$i])<<(int)$j;
if(DEBUG) echo "$next\n";
    $int |= $next;
  }
  $i = ((int)$int) % (pow(10,(int)$pow));
  return substr((string)(int)str_pad(abs($i),10,STR_PAD_RIGHT,'0'),-6);
}

if(DEBUG) echo hotp(array(0x1f,0x86,0x98,0x69,0x0e,0x02,0xca,0x16,0x61,0x85,0x50,0xef,0x7f,0x19,0xda,0x8e,0x94,0x5b,0x55,0x5a))."\n";

function totp($secret,$div = 30,$test=false){
  static $c;
  if($test && !isset($c))
    $c = mt_rand(0,time());
  else
    $c = time();
  $hash = unpack('C*',hash_hmac('sha1',"".floor($c/$div),$secret,true));
if(DEBUG) echo trim($secret,"\n")."\n";
if(DEBUG) echo floor($c/$div)."\n";
if(DEBUG) echo implode(',',$hash)."\n";
  $c+=30;
  $tmp = hotp($hash);
  return $tmp;
}

