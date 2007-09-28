# -*-python-*-
from sys import platform

import os
import SCons

def ENV_update(tgt_ENV, src_ENV):
    for K in src_ENV.keys():
        if K in [ 'PATH', 'LD_LIBRARY_PATH',
                  'LIB', 'INCLUDE', 'USERPROFILE', 'HOME' ]:
	    if K in tgt_ENV.keys():
		tgt_ENV[K]=SCons.Util.AppendPath(tgt_ENV[K], src_ENV[K])
	    else:
		tgt_ENV[K]=src_ENV[K]

def ENV_print(tgt_ENV):
    for K in tgt_ENV.keys():
	print K, " = ", tgt_ENV[K]

env = Environment(tools= ['default'], toolpath=['m4'])

ENV_update(env[ 'ENV' ], os.environ)
#ENV_print(env[ 'ENV' ])

print "Platform: ", env['PLATFORM']

if env['PLATFORM'] == 'win32':
    env[ 'USERHOME' ] = os.environ[ 'USERPROFILE' ]
else:
    env[ 'USERHOME' ] = os.environ[ 'HOME' ]

if env['PLATFORM'] == 'win32':
    env.Append(CXXFLAGS = '/EHsc /MDd' )
    env.Append(SYSTEMROOT = os.environ['SYSTEMROOT'])

header_env_string = 'INCLUDE'

if 'INCLUDE' in env['ENV'].keys():
   env.Append(CPPPATH = env['ENV'][ 'INCLUDE' ].split( os.pathsep ) )

if 'LIB' in env['ENV'].keys():
   env.Append(LIBPATH = env['ENV'][ 'LIB' ].split( os.pathsep ) )

env.Append(CPPPATH = ['#src'])
env.Append(CPPPATH = ['#.'])

def AutoConfigure(env):
    conf = Configure(env)
    if env['PLATFORM'] == 'win32':
	if not conf.CheckCXXHeader('Windows.h'):
	    print 'The system header "Windows.h" could not be found'
	    print 'please make sure the ', header_env_string, ' environment'
	    print 'variable is set correctly'
	    Exit(1)
	else:
	    env.Append(CPPDEFINES={'HAVE_WINDOWS_H' : '1'})
	if not conf.CheckLib('uuid'):
	    print 'The system library "uuid.lib" could not be found'
	    print 'please make sure the LIB environment'
	    print 'variable is set correctly'
	    Exit(1)
	if not conf.CheckLib('libcp'):
	    print 'The system library "libcp.lib" could not be found'
	    print 'please make sure the LIB environment'
	    print 'variable is set correctly'
	    Exit(1)
	if not conf.CheckCHeader('Winsock2.h'):
	    print 'The system header "Winsock.h" could not be found'
	    print 'please make sure the ', header_env_string, ' environment'
	    print 'variable is set correctly'
	    Exit(1)
	if not conf.CheckLib('ws2_32'):
	    print 'The system library "ws2_32.lib" could not be found'
	    print 'please make sure the LIB environment'
	    print 'variable is set correctly'
	    Exit(1)
	else:
	    env.Append(CCFLAGS = '-DHAVE_WINSOCK2_H=1')
    else:
	if conf.CheckCHeader('sys/socket.h'):
	    env.Append(CCFLAGS = '-DHAVE_SYS_SOCKET_H=1')
	if conf.CheckType('SOCKET', '#include <sys/socket.h>\n'):
	    env.Append(CCFLAGS = '-DHAVE_SOCKET=1')
	if conf.CheckType('socklen_t', '#include <sys/socket.h>\n'):
	    env.Append(CCFLAGS = '-DHAVE_SOCKLEN_T=1')
	if conf.CheckCHeader('netinet/in.h'):
	    env.Append(CCFLAGS = '-DHAVE_NETINET_IN_H=1')
	if conf.CheckCHeader('arpa/inet.h'):
	    env.Append(CCFLAGS = '-DHAVE_ARPA_INET_H=1')
	if conf.CheckCHeader('netdb.h'):
	    env.Append(CCFLAGS = '-DHAVE_NETDB_H=1')
	if conf.CheckCHeader('sys/param.h'):
	    env.Append(CCFLAGS = '-DHAVE_SYS_PARAM_H=1')
	if conf.CheckCHeader('sys/time.h'):
	    env.Append(CCFLAGS = '-DHAVE_SYS_TIME_H=1')
	if not conf.CheckCHeader('pwd.h'):
	    print 'The system header "pwd.h" could not be found'
	    print 'please make sure the ', header_env_string, ' environment'
	    print 'variable is set correctly'
	    Exit(1)
	else:
	    env.Append(CPPDEFINES={'HAVE_PWD_H' : '1'})
	if not conf.CheckLib('ltdl'):
	    print 'The libtool library "libltdl.a" could not be found'
	    print 'please make sure it\'s installed and the LIB environment'
	    print 'variable is set correctly'
	    Exit(1)
	
	if conf.CheckCXXHeader('zlib.h'):
	    if conf.CheckLib('libz'):
		env.Append(CCFLAGS = '-DHAVE_LIBZ=1')
		env['HAVE_LIBZ'] = True
	    else:
		env['HAVE_LIBZ'] = False
		print "Warning: the library 'libz' could ne found."
		print "This means the rcssgz library will be build without any"
		print "compression/decompression capabilities"
		print "if you wish to fully enable the rcssgz library, please install"
		print "zlib (www.zlib.net) and make sure the LIB environment"
		print "variable is set correctly"
	else:
	    env['HAVE_LIBZ'] = False
	    print "Warning: the header 'zlib.h' could not be found."
	    print "This means the rcssgz library will be build without any"
	    print "compression/decompression capabilities"
	    print "if you wish to fully enable the rcssgz library, please install"
	    print "zlib (www.zlib.net) and make sure the ", header_env_string, " environment"
	    print "variable is set correctly"


	if not conf.CheckCXXHeader('utility'):
	    print 'The STL header "utility" could not be found'
	    print 'please make sure the ', header_env_string, ' environment'
	    print 'variable is set correctly'
	    Exit(1)
	if not conf.CheckCXXHeader('boost/version.hpp'):
	    print 'boost/version.hpp could not be found'
	    print 'please make sure boost (www.boost.org) is installed'
	    print 'and the ', header_env_string, ' environment variable is set correctly'
	    Exit(1)
		
	if env['PLATFORM'] != 'win32':
	    if not conf.CheckLib('boost_filesystem'):
		print 'The boost library "boost_filesystem" could not be found'
		print 'please make sure boost is installed and the LIB environment'
		print 'variable is set correctly'
		Exit(1)


	if conf.CheckCXXHeader('sstream'):
	    env.Append(CCFLAGS = '-DHAVE_SSTREAM=1')
		
	if conf.CheckCXXHeader('cppunit/Test.h'): 
	    if conf.CheckLib('cppunitd'):
		env['HAVE_CPPUNIT'] = True
		env['CPPUNIT_LIB'] = 'cppunitd'
	    elif conf.CheckLib('cppunit'):
		env['HAVE_CPPUNIT'] = True
		env['CPPUNIT_LIB'] = 'cppunit'
	    else:
		env['HAVE_CPPUNIT'] = False
	else:
	    env['HAVE_CPPUNIT'] = False

	if conf.CheckFunc('mkstemp'):
	    env.Append(CPPDEFINES={'HAVE_MKSTEMP' : '1'})
    env = conf.Finish()

def envInit(package_name, package_version, bug_list, package):
    env = Environment(ENV = os.environ)

AutoConfigure(env)
 
env[ 'package' ] = 'rcssserver'

env[ 'DESTDIR' ] = ARGUMENTS.get( 'DESTDIR', '' )

if env['PLATFORM'] == 'win32':
    if len( env[ 'DESTDIR' ] ) > 0:
	env[ 'default_prefix' ] = os.path.join( env[ 'DESTDIR' ], 'C_\\Program Files' )
    else:
	env[ 'default_prefix' ] = 'C:\\Program Files'
else:
    if len( env[ 'DESTDIR' ] ) > 0:
	env[ 'default_prefix' ] = os.path.join( env[ 'DESTDIR' ], '/usr/local' )
    else:
	env[ 'default_prefix' ] = '/usr/local'



env[ 'prefix' ] = ARGUMENTS.get( 'prefix', env[ 'default_prefix' ] )

print "installation prefix: ", env[ 'prefix' ]

env[ 'exec_prefix' ] = ARGUMENTS.get( 'exec_prefix', env[ 'prefix' ] )
env[ 'bindir' ] = ARGUMENTS.get( 'bindir',
     	      	  		 os.path.join( env[ 'exec_prefix' ], 'bin' ) )
env[ 'sbindir' ] = ARGUMENTS.get( 'sbindir', 
     	       	   		  os.path.join( env[ 'exec_prefix' ], 'sbin' ) )
env[ 'libexecdir' ] = ARGUMENTS.get( 'libexecdir', 
     	       	   		     os.path.join( env[ 'exec_prefix' ], 'libexec' ) )
env[ 'datadir' ] = ARGUMENTS.get( 'datadir', 
     	       	   		  os.path.join( env[ 'prefix' ], 'share' ) )
env[ 'sysconfdir' ] = ARGUMENTS.get( 'sysconfdir', 
				     os.path.join( env[ 'prefix' ], 'etc' ) )
env[ 'sharedstatedir' ] = ARGUMENTS.get( 'sharedstatedir', 
					 os.path.join( env[ 'prefix' ], 'com' ) )
env[ 'localstatedir' ] = ARGUMENTS.get( 'localstatedir', 
					os.path.join( env[ 'prefix' ], 'var' ) )
env[ 'libdir' ] = ARGUMENTS.get( 'libdir', 
				 os.path.join( env[ 'exec_prefix' ], 'lib' ) )
env[ 'includedir' ] = ARGUMENTS.get( 'includedir', 
				     os.path.join( env[ 'prefix' ], 'include' ) )
env[ 'infodir' ] = ARGUMENTS.get( 'infodir', 
     	       	   		  os.path.join( env[ 'prefix' ], 'info' ) )
env[ 'mandir' ] = ARGUMENTS.get( 'mandir', 
				 os.path.join( env[ 'prefix' ], 'man' ) )
env[ 'pkgdatadir' ] = os.path.join( env[ 'datadir' ], env[ 'package' ] )
env[ 'pkglibdir' ] = os.path.join( env[ 'libdir' ], env[ 'package' ] )
env[ 'pkgincludedir' ] = os.path.join( env[ 'includedir' ], env[ 'package' ] )

env[ 'moduledir' ] = os.path.join( env[ 'pkglibdir' ], 'modules' )

#env.Append(CPPDEFINES={'MODULEDIR' : '\\"'+env[ 'moduledir' ]+'\\"'})

env.Append(CPPPATH = env['includedir'] )
env.Append(LIBPATH = env['libdir'] )
env.Append(PATH = env['bindir'] )
env.Append(PATH = env['sbindir'] )


env.Help("""
Type: 'scons' to build rcssserver

Installation directories:
  prefix=PREFIX         install architecture-independent files in PREFIX
                        [""" + env['default_prefix'] + """]
  exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX
                        [PREFIX]

By default, `make install' will install all the files in
`""" + os.path.join( env['default_prefix'], 'bin' ) + """', `""" +  os.path.join( env['default_prefix'], 'lib' )  + """' etc.  You can specify
an installation prefix other than `""" + env['default_prefix'] + """' using `prefix',
for instance `prefix=""" + env[ 'USERHOME' ] + """'.

For better control, use the options below.

Fine tuning of the installation directories:
  bindir=DIR           user executables [EPREFIX/bin]
  sbindir=DIR          system admin executables [EPREFIX/sbin]
  libexecdir=DIR       program executables [EPREFIX/libexec]
  datadir=DIR          read-only architecture-independent data [PREFIX/share]
  sysconfdir=DIR       read-only single-machine data [PREFIX/etc]
  sharedstatedir=DIR   modifiable architecture-independent data [PREFIX/com]
  localstatedir=DIR    modifiable single-machine data [PREFIX/var]
  libdir=DIR           object code libraries [EPREFIX/lib]
  includedir=DIR       C header files [PREFIX/include]
  infodir=DIR          info documentation [PREFIX/info]
  mandir=DIR           man documentation [PREFIX/man]

""")		       

env.Append(CPPDEFINES={'VERSION' : '"\\"10.1.0\\""'})

plg_env = env.Copy()
if env[ 'PLATFORM' ] == 'darwin':
    plg_env[ 'SHLINKFLAGS' ] = '$LINKFLAGS -bundle -flat_namespace -undefined suppress'
    plg_env[ 'SHLIBSUFFIX' ] = '.so'
    
unit_env = env.Copy()
if env['HAVE_CPPUNIT']:
    unit_env['test_main']=unit_env.Object('testmain.cpp')

prog_env = env.Copy()

shlib_env = env.Copy()
shlib_env.Append(CPPDEFINES={'RCSSSERVER_EXPORTS' : '1'})


Export('env unit_env prog_env plg_env shlib_env')

SConscript(['src/SConscript'])

 
