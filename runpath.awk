/\${[^}]*}/ { 
while( match( $0, /\\\${[^}]*}/ ) )
{
token = substr( $0, RSTART+3, RLENGTH-4 ); 
token_length = RLENGTH-4;
"grep ^" token " libtool" | getline tmp;
x = match( tmp, /".*"/ ); 
rep = substr( tmp, RSTART+1, RLENGTH-2 );
idx = index( $0, token );
while( idx )
{
    $0 = substr( $0, 0, idx - 4 ) rep substr( $0, idx + 1 + token_length); 
    idx = index( $0, token );
}
}
print substr( $0, 2, length() -2 );
}
