<?php
/*
* This is a Perl database wrapper for PHP (So to speak, More like a hack really)
* trring to compile php to use ANY Oracle 32 client on Freebsd 6.x was a PITA
* since perl has no problem, i came up with this file
*
* Ref:
* http://mrtenente.infosys.lt/blog/?p=22
*
* Description: This extension embeds Perl Interpreter into PHP. It allows execute Perl files, evaluate Perl code, access Perl variables and
*             instantiate Perl objects.
* http://pecl.php.net/package/perl
*
* Using Perl Code from PHP
* http://devzone.zend.com/node/view/id/1712
*
*
* Fri Aug 17 15:17:16 CDT 2007
* - nocon
* nocon@darkflame.net
*
* Example:
*
* include '/path/to/this/filename.inc';
* $DataBase = new OracleDatabase();
* $Sql = "update table set something = 'weeeeeeee!' where id = 123456";
* $Data = $DataBase->Insert($Sql);
*
* $Sql = "select * from table where id = 123456";
* $Data = $DataBase->Select($Sql);
*
* print "<pre>";
* print_r($Data);
* print "</pre>";
*
*
* ToDo (When/if i feel like it)
*   - Cleanup bla bla and remove redundant code
*/


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*
*  Oracle Database class routines
*
*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OracleDatabase {

       var $arrData            = array();
       var $arrErrors          = array();
       var $arrMessages        = array();

      function OracleDatabase() {
           //Oracle Default Server Info
           $this->arrData['ORACLE_HOME']   = '/usr/local/oracle7';

           $this->arrData['DB_SERVER'] = '';
           $this->arrData['DB_PORT']   = '';
           $this->arrData['DB_SID']    = '';
           $this->arrData['DB_USER']   = '';
           $this->arrData['DB_PASS']   = '';
      }


       function Select($strSqlStateMent) {
                if(!$strSqlStateMent) { $this->arrErrors[] = "[".__FUNCTION__ ."] Missing sql statement.\n"; }
                if(count($this->arrErrors)) { return; }

                   $perl = new Perl();
                   $perl->eval('
                     use strict;
                     use DBI;

                     package OracleDB;
                     sub new {

                             my ($class) = shift;
                             my $self = {};
                             bless($self, $class);

                             $ENV{"ORACLE_HOME"} = "'.$this->arrData['ORACLE_HOME'].'";

                         return $self;
                      }

                      sub Select {
                              my $self = shift;
                              my $SqlStatement = shift;

                              my $server   = "'.$this->arrData['DB_SERVER'].'";
                              my $port     = "'.$this->arrData['DB_PORT'].'";
                              my $sid      = "'.$this->arrData['DB_SID'].'";
                              my $username = "'.$this->arrData['DB_USER'].'";
                              my $password = "'.$this->arrData['DB_PASS'].'";

                              my $dbh = DBI->connect( "dbi:Oracle:host=$server;sid=$sid;port=$port", $username, $password,
                                                   { RaiseError => 1, AutoCommit => 0, PrintError => 1, LongReadLen => 1000000 }
                                                 ) || die "Database connection not made: $DBI::errstr";


                             my $Database = $dbh;

                              $SqlStatement = qq{$SqlStatement};
                              my $Sth = $Database->prepare($SqlStatement) || die "Error Preparing SQL Statment.\n";
                              if(!$Sth->execute()) {
                                  $Sth->finish();
                                  $Database->disconnect();
                                  return;
                              }

                               my $NumFields = $Sth->{NUM_OF_FIELDS};
                               my @data;
                               my $RowNum = 0;
                               my $x;

                                  while(@data = $Sth->fetchrow_array()) {
                                    for($x = 0; $x < $NumFields; $x++ ) {
                                           @data[$x] =~ s/\s+/ /g;
                                           $self->{data}->[$RowNum]->{uc($Sth->{NAME}->[$x])} = "@data[$x]";
                                      }

                                    $RowNum++;
                                  }

                                 $Sth->finish();
                                 $Database->disconnect();

                               return;
                      }
                       ');

                     $perl = new Perl("OracleDB");

                     try {
                           $perl->Select($strSqlStateMent);
                         }
                           catch (PerlException $exception) {
                           echo "Perl error: " . $exception->getMessage() . " ";
                         }

                      $DbResults = $perl->data;
                      unset($perl->data);

                      if(count($DbResults) == 1) {
                               $DbResults = array_shift($DbResults);
                      }

                    return $DbResults;
                 }

             function Insert($strSqlStateMent) {
                       if(!$strSqlStateMent) { $this->arrErrors[] = "[".__FUNCTION__ ."] Missing sql statement.\n"; }
                       if(count($this->arrErrors)) { return; }


                        $perl = new Perl();
                        $perl->eval('
                          use strict;
                          use DBI;

                          package OracleDB;
                          sub new {

                                  my ($class) = shift;
                                  my $self = {};
                                  bless($self, $class);

                                  $ENV{"ORACLE_HOME"} = "'.$this->arrData['ORACLE_HOME'].'";

                              return $self;
                           }

                           sub Insert {
                              my $self = shift;
                              my $strSqlStateMent = shift;

                              my $server   = "'.$this->arrData['DB_SERVER'].'";
                              my $port     = "'.$this->arrData['DB_PORT'].'";
                              my $sid      = "'.$this->arrData['DB_SID'].'";
                              my $username = "'.$this->arrData['DB_USER'].'";
                              my $password = "'.$this->arrData['DB_PASS'].'";

                              my $dbh = DBI->connect( "dbi:Oracle:host=$server;sid=$sid;port=$port", $username, $password,
                                                   { RaiseError => 1, AutoCommit => 0, PrintError => 1, LongReadLen => 1000000 }
                                                 ) || die "Database connection not made: $DBI::errstr";


                             my $Database = $dbh;

                             my $Sql = qq{$strSqlStateMent};
                             my $Sth = $Database->prepare($Sql) || die "Error Preparing SQL Statment.\n";

                             if(!$Sth->execute()) {
                                 $Sth->finish();
                                 $Database->rollback();
                                 $Database->disconnect();
                                 return;
                             }

                             $Sth->finish();
                             $Database->commit();
                             $Database->disconnect();

                             return;
                      }
                       ');


                        $perl = new Perl("OracleDB");

                         try {
                               $perl->Insert($strSqlStateMent);
                             }
                               catch (PerlException $exception) {
                               echo "Perl error: " . $exception->getMessage() . " ";
                             }
                return;
             }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*
*  MySql Database class routines
*
*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MySqlDatabase {
       var $arrData            = array();
       var $arrErrors          = array();
       var $arrMessages        = array();

      function MySqlDatabase() {

                //Database Default Options
                $this->arrData['DB_SERVER'] = '';
                $this->arrData['DB_BASE']   = '';
                $this->arrData['DB_USER']   = '';
                $this->arrData['DB_PASS']   = '';
      }

      function Select($strSqlStateMent) {
                if(!$strSqlStateMent) { $this->arrErrors[] = "[".__FUNCTION__ ."] Missing sql statement.\n"; }
                if(count($this->arrErrors)) { return; }


                   $perl = new Perl();
                   $perl->eval('
                     use strict;
                     use DBI;

                     package MySqlDB;
                     sub new {

                             my ($class) = shift;
                             my $self = {};
                             bless($self, $class);

                         return $self;
                      }

                      sub Select {
                              my $self = shift;
                              my $SqlStatement = shift;

                              my $server   = "'.$this->arrData['DB_SERVER'].'";
                              my $database = "'.$this->arrData['DB_BASE'].'";
                              my $username = "'.$this->arrData['DB_USER'].'";
                              my $password = "'.$this->arrData['DB_PASS'].'";

                              my $dbh = DBI->connect( "dbi:mysql:$database:$server", $username, $password,
                                                      { RaiseError => 1, AutoCommit => 0, PrintError => 1, LongReadLen => 1000000 }
                                                    ) || die "Database connection not made: $DBI::errstr";


                             my $Database = $dbh;

                              $SqlStatement = qq{$SqlStatement};
                              my $Sth = $Database->prepare($SqlStatement) || die "Error Preparing SQL Statment.\n";
                              if(!$Sth->execute()) {
                                  $Sth->finish();
                                  $Database->disconnect();
                                  return;
                              }

                               my $NumFields = $Sth->{NUM_OF_FIELDS};
                               my @data;
                               my $RowNum = 0;
                               my $x;

                                  while(@data = $Sth->fetchrow_array()) {
                                    for($x = 0; $x < $NumFields; $x++ ) {
                                           @data[$x] =~ s/\s+/ /g;
                                           $self->{data}->[$RowNum]->{uc($Sth->{NAME}->[$x])} = "@data[$x]";
                                      }

                                    $RowNum++;
                                  }

                                 $Sth->finish();
                                 $Database->disconnect();

                               return;
                      }
                       ');


                     $perl = new Perl("MySqlDB");

                     try {
                           $perl->Select($strSqlStateMent);
                         }
                           catch (PerlException $exception) {
                           echo "Perl error: " . $exception->getMessage() . " ";
                         }

                      $DbResults = $perl->data;
                      unset($perl->data);

                      if(count($DbResults) == 1) {
                               $DbResults = array_shift($DbResults);
                      }

                    return $DbResults;
                }


                function Insert($strSqlStateMent) {
                       if(!$strSqlStateMent) { $this->arrErrors[] = "[".__FUNCTION__ ."] Missing sql statement.\n"; }
                       if(count($this->arrErrors)) { return; }


                        $perl = new Perl();
                        $perl->eval('
                          use strict;
                          use DBI;

                          package MySqlDB;
                          sub new {

                                  my ($class) = shift;
                                  my $self = {};
                                  bless($self, $class);

                              return $self;
                           }

                           sub Insert {
                              my $self = shift;
                              my $strSqlStateMent = shift;

                              my $server   = "'.$this->arrData['DB_SERVER'].'";
                              my $database = "'.$this->arrData['DB_BASE'].'";
                              my $username = "'.$this->arrData['DB_USER'].'";
                              my $password = "'.$this->arrData['DB_PASS'].'";

                              my $dbh = DBI->connect( "dbi:mysql:$database:$server", $username, $password,
                                                      { RaiseError => 1, AutoCommit => 0, PrintError => 1, LongReadLen => 1000000 }
                                                    ) || die "Database connection not made: $DBI::errstr";



                             my $Database = $dbh;

                             my $Sql = qq{$strSqlStateMent};
                             my $Sth = $Database->prepare($Sql) || die "Error Preparing SQL Statment.\n";

                             if(!$Sth->execute()) {
                                 $Sth->finish();
                                 $Database->rollback();
                                 $Database->disconnect();
                                 return;
                             }

                             $Sth->finish();
                             $Database->commit();
                             $Database->disconnect();

                             return;
                      }
                       ');


                        $perl = new Perl("MySqlDB");

                         try {
                               $perl->Insert($strSqlStateMent);
                             }
                               catch (PerlException $exception) {
                               echo "Perl error: " . $exception->getMessage() . " ";
                             }
                return;
             }
}


?>

