<?


function expires($time) {
  // default is to expire in 1/2 hour
  return ($time+1800);
}

function is_logged_in($user, $pass) {
  global $conn; 
  $currtime=time();
  $newexpires=expires($currtime);
  $ila_res = pg_exec
    ($conn, 
     "select * from session where username='$user' " . 
     "  and password='$pass' and expires>$currtime");
  pg_exec
    ($conn, 
     "delete from session where username='$user' " . 
     "  and password='$pass' and expires<$currtime");
  if(pg_numrows($ila_res)) {
    $rrow=pg_fetch_row($ila_res, 0);
    $sess_id=$rrow[0];
    pg_exec
      ($conn, 
       "update session set expires=$newexpires where session_id='$sess_id'");
    return $sess_id;
  } else {
    return 0;
  }
}

function u_logout($user, $pass) {
  global $conn;
  pg_exec($conn, 
	  "delete from session where username='$user' and password='$pass'");
}

function s_logout($sess_id) {
  global $conn;
  pg_exec($conn, 
	  "delete from session where session_id='$sess_id'");
}

function s_lookup($sess_id) {
  global $conn;
  $result = pg_exec
    ($conn, 
     "select username, password, expires from session where session_id='$sess_id'");
  if(pg_numrows($result)) {
    $row=pg_fetch_row($result, 0);
    if($row[2]<time()) {
      return 0;
    } else {
      return $row;
    }
  } else {
    return 0;
  }
}

function s_renew($sess_id) {
  global $conn;
  $newexpires=expires(time());
  pg_exec
    ($conn, 
     "update session set expires=$newexpires where session_id='$sess_id'");
}

function authenticate($user, $pass)  {
  // authenticate first checks to see if the user is already logged
  // in.  If she is, then it returns the session ID already given.
  // Otherwise, it calls authenticate_user(), which makes an IMAP connection
  // to the mail server and checks the username and password.  If it
  // is a correct login, create a session ID based upon the username,
  // password, and UNIX timestamp, insert a row in the database with
  // the necessary info and expiration time and return said session
  // ID. Otherwise, return 0.

  global $conn;
  $still_logged_in=is_logged_in($user, $pass);
  $expiredate=expires(time());
  $from_host=$HOST;

  if($still_logged_in) {
    return $still_logged_in;
  }

  if(authenticate_user($user, $pass)) {
    $user_pass_str = $user . $pass . time();
    $sess_id = md5($user_pass_str);
    pg_exec($conn,
	    "insert into session (session_id, username, password, expires)" .
	    "  values ('$sess_id', '$user', '$pass', $expiredate)");
    return $sess_id;
  } else {
    return 0;
  }
}



if($addressbook_user != "") {
  $redir_to="index.php3";
  include("redir.php3");
  exit();
}
?>
