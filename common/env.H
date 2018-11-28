/*
 *  This file is part of the Home2L project.
 *
 *  (C) 2015-2018 Gundolf Kiefer
 *
 *  Home2L is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Home2L is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Home2L. If not, see <https://www.gnu.org/licenses/>.
 *
 */


#ifndef _ENV_
#define _ENV_

// This file is SWIG-readable and part of the Python library.


/** @file
 * @defgroup env Environment
 * Access configuration parameters and query environment properties.
 *
 * @{
 */

#ifdef SWIG
%pythoncode %{
## @defgroup env Environment
## Access configuration parameters and query environment properties.
##
## More detailed information on these functions can be found in the
## [*C/C++ API documentation*](../home2l-api_c/group__env.html).
## @{
%}
#endif


#ifndef SWIG

#include "base.H"

#include <stdio.h>

#endif // SWIG



// ********************** Global settings ***********************


/// @defgroup env_settings Static Settings
/// Global compile-time settings.
/// @{


#define HOME2L_USER "home2l"    ///< User and group name of the Home2L user (for background processes).


/// @}  // @defgroup




// ********************** Misc Helpers *****************************************


/// @defgroup env_helpers Helpers
/// Direct access to basic environment properties and pathname helpers.
/// @{


/// @name Environment ...
/// @{

const char *EnvBuildOS ();        ///< Operating software ("Debian", "Android", ...)
const char *EnvBuildArch ();      ///< Operating architecture (e.g. "i386", "armhf", ...)

const char *EnvMachineName ();    ///< Machine name (network host name)

const char *EnvExecPathName ();   ///< Full path name of the executable (== argv[0]).
  ///< **Note:** The return values of EnvExecPathName() and EnvExecName() may be undefined (== NULL),
  ///<           e.g. if the main program is a Python script or an Android app (TBD: FIX?).
const char *EnvExecName ();       ///< File name of the executable without path.

const char *EnvInstanceName ();
  /**<  Instance name - the identifier for the running process.
   *
   * The instance name weakly identifies the running process as follows:
   *
   * * By default ('EnvInit' called with 'argv[0][0] != '/'), it is set to the executable
   *   name (without leading "home2l-").
   *   By calling 'EnvInit' with 'argv[0][0] != '/', it can be set to any given name,
   *   in which case EnvExecPathName() will return NULL. In this case 'argv[0]' must
   *   refer to the instance full name.
   *
   * * In a correct installation and during normal operation, no two processes with the same
   *   instance name shall be executed simultaneously. This must be considered in the per-host
   *   setup and when writing tools for which multiple running instances regularly may make sense.
   *
   * * The required uniqueness (previous note) is weak in the sense that no technical mechanisms
   *   exist to enforce it and no software may rely on it. An example is the case where an
   *   administrator starts two Home2l shells simultaneously, which may well act as a single
   *   instance.
   *
   * * Code requiring strict uniqueness should use the PID as an additional criterion
   *   (presently done so in the *Resources* library to identify non-server hosts).
   */

const char *EnvDroidId ();        ///< Droid ID

int EnvPid ();                    ///< Operating system's process identifier (PID).
bool EnvHaveTerminal ();          ///< 'true', if the application has been started from an interactive terminal.

/// @}



/// @name Directories ...
/// @{

const char *EnvHome2lRoot ();     ///< Root installation path: all static data (blob + etc).
const char *EnvHome2lVar ();      ///< 'var' root path: dynamic data (saved over reboots).
const char *EnvHome2lTmp ();      ///< 'tmp' root path: dynamic data (not need to save over reboots).

#ifndef SWIG

const char *EnvGetHome2lRootPath (CString *ret, const char *relOrAbsPath);
  ///< Return an absolute path name in the root domain.
  ///< A path starting with '/' is considered already absolute and is returned unchanged, but normalized.
const char *EnvGetHome2lVarPath (CString *ret, const char *relOrAbsPath);
  ///< Return an absolute path name in the 'var' domain.
const char *EnvGetHome2lTmpPath (CString *ret, const char *relOrAbsPath);
  ///< Return an absolute path name in the 'tmp' domain.

#else // SWIG

%feature("docstring") EnvGetHome2lRootPath "Return an absolute path name in the root domain.\n\n" \
  "Path starting with '/' are considered already absolute and are\n" \
  "returned unchanged, but normalized.\n"
%feature("docstring") EnvGetHome2lVarPath "Return an absolute path name in the 'var' domain.\n\n" \
  "Path starting with '/' are considered already absolute and are\n" \
  "returned unchanged, but normalized.\n"
%feature("docstring") EnvGetHome2lTmpPath "Return an absolute path name in the 'tmp' domain.\n\n" \
  "Path starting with '/' are considered already absolute and are\n" \
  "returned unchanged, but normalized.\n"

%newobject EnvGetHome2lRootPath (const char *);
%newobject EnvGetHome2lVarPath (const char *);
%newobject EnvGetHome2lTmpPath (const char *);
%inline %{
static inline const char *EnvGetHome2lRootPath (const char *relOrAbsPath) { CString s; EnvGetHome2lRootPath (&s, relOrAbsPath); return s.Disown (); }
static inline const char *EnvGetHome2lVarPath (const char *relOrAbsPath) { CString s; EnvGetHome2lVarPath (&s, relOrAbsPath); return s.Disown (); }
static inline const char *EnvGetHome2lTmpPath (const char *relOrAbsPath) { CString s; EnvGetHome2lTmpPath (&s, relOrAbsPath); return s.Disown (); }
%} // inline

#endif // SWIG

bool EnvMkVarDir (const char *relOrAbsPath);
  ///< Prepare (create) a directory in the 'var' domain.
  ///< @param relOrAbsPath is either an absolute path or a path relative to the domain.
  ///< On error, a warning is emitted and 'false' is returned.
bool EnvMkTmpDir (const char *relOrAbsPath);
  ///< Prepare (create) a directory in the 'tmp' domain.

/// @}  // @name


/// @}  // @defgroup





// ********************** Environment dictionary *****************************


/// @defgroup env_general General
/// Init/done and persistence.
/// @{


/// @name Init/Done ...
/// @{
void EnvInit (int argc, char **argv, const char *specOptionsUsage = NULL, const char *instanceName = NULL);
void EnvDone ();
/// @}


/// @name Persistence ...
/// @{
void EnvInitPersistence (bool writeThrough = false, const char *_varFileName = NULL);
  /**< Enable the persistence of all environment variables starting with "var.*".
   *
   * They will be stored to a file passed as '_varFileName', which should be
   * pathname relative to the "var" directory. By default, "home2l-<instance name>.conf"
   * is used. It is discouraged to pass an absolute path there.
   *
   * If 'writeThrough == true', the file is written back with each EnvPut() call changing
   * any "var.*" variable. If 'writeThrough == false', the file is only written back
   * on shutdown ( EnvDone() ) or on a explicit flush ( EnvFlush() ).
   *
   * Persistent variables are (tried to be) read from the filesystem on this call,
   * and they may override any settings in the main configuration file.
   */
void EnvFlush ();   ///< Write back any persistent variables now.
/// @}


/// @}  // @defgroup



/// @defgroup env_dictionary Dynamic Settings
/// Accessing and managing the environment dictionary.
/// @{


/// @name Primitive Get/Put ...
/// @{

#ifdef SWIG
%feature("docstring") EnvGet "Get a Home2L configuration variable from the environment."
#endif
const char *EnvGet (const char *key);
  ///< Generically read a setting; Returns 'NULL' on non-existence and never prints warnings.
  /// 'key' can optionally be a sequence of keys separated by a colon (":"). In this case,
  /// the keys are tried in sequential order and the first available value is returned.

const char *EnvPut (const char *key, const char *value);
  ///< Generically put a string into the dictionary; Passing 'value == NULL' removes the entry.
  /// @return Pointer the copy of the value as stored in the database.
  /// **Note:** It is allowed to call this method before 'EnvInit' to pre-initialize environment settings.
  ///       This feature is e.g. used for the Android initialization to set the hostname.
const char *EnvPut (const char *key, int value);
  ///< Put an integer or boolean variable into the dictionary.
  /// @return Pointer the copy of the value as stored in the database.

/// @}


#ifndef SWIG

/// @name Get with type ...
/// These functions get an environment parameter and convert them to a given type.
/// For each type, there are two variants:
/// 1. An advanced function returning the success status and - only on success - writing the result back via a pointer
/// 2. A simple function returning that type; On error, the type's default value is returned.
/// In all variants, a warning is emitted on error or non-existence, if the parameter 'warn' is set.
///
/// **Note:** The functions with implicit translation capabilities ('EnvGetPath*' and 'EnvGetHost*') convert
///   their strings in place. This means:
///   1. Later retrievals by EnvGet() will return the absolute path, not the original string, e.g. from the configuration file.
///   2. These functions may only be called from the main thread.
/// @{

bool EnvGetString (const char *key, const char **ret, bool warn = false);
  ///< Read a string (advanced variant).
const char *EnvGetString (const char *key, const char *defaultVal, bool warn = false);
  ///< Read a string (simple variant).
static inline const char *EnvGetString (const char *key, bool warn) { return EnvGetString (key, (const char *) NULL, warn); }
  ///< Read a string (simple variant) without default.
static inline const char *EnvGetString (const char *key) { return EnvGet (key); }
  ///< Accelerator for 'EnvGetString (key, warn = false)'.

bool EnvGetPath (const char *key, const char **ret, bool warn = false);
  ///< Read a path, eventually convert value to an absolute path before (advanced).
const char *EnvGetPath (const char *key, const char *defaultVal, bool warn = false);
  ///< Read a path, eventually convert value to an absolute path before (simple).
static inline const char *EnvGetPath (const char *key, bool warn = false) { return EnvGetPath (key, (const char *) NULL, warn); }
  ///< Read a path (simple variant) without default.
bool EnvGetHostAndPort (const char *key, CString *retHost, int *retPort = NULL, int defaultPort = 0, bool warn = false);
  ///< Get a host + port combination; host names are translated using the 'rc.resolve.\<name\>' settings.
static inline bool EnvGetHost (const char *key, CString *retHost, bool warn = false) { return EnvGetHostAndPort (key, retHost, NULL, 0, warn); }
  ///< Get a host name; host names are translated using the 'rc.resolve.\<name\>' settings.


bool EnvGetInt (const char *key, int *ret, bool warn = false);     ///< Read an integer (advanced).
int EnvGetInt (const char *key, int defaultVal = 0, bool warn = false);           ///< Read an integer (simple).

bool EnvGetFloat (const char *key, float *ret, bool warn = false); ///< Read a float value (advanced).
float EnvGetFloat (const char *key, float defaultVal = 0.0, bool warn = false);   ///< Read a float value (simple).

bool EnvGetBool (const char *key, bool *ret, bool warn = false);   ///< Read a boolean value (advanced).
bool EnvGetBool (const char *key, bool defaultVal = false, bool warn = false);    ///< Read a boolean value (simple).

/// @}

#endif // SWIG




#ifndef SWIG

/// @name Advanced access to the dictionary ...
/// @{
void EnvGetPrefixInterval (const char *prefix, int *retIdx0, int *retIdx1);
const char *EnvGetKey (int idx);
const char *EnvGetVal (int idx);

CDictRaw *EnvGetKeySet ();  ///< for direct key listing; manipulations of the returned object are strictly forbidden.
/// @}

#endif // SWIG


/// @}  // @defgroup





// ************ Parameter declaration with automatic documentation *************


/** @defgroup env_declaration Declaration
 * Declaring environment parameters used in your code with automatic documentation.
 *
 * For declaring variables reflecting environment settings, the following macros should be used, and the variables
 * should be declared globally. This way, automatic tools can generate a documentation of the environment parameters.
 * The macros take care properly initializing the variables in 'EnvInit', the user does not need to call 'EnvGet...'
 * for these variables. If this is not possible (e.g. because the key is variable or the type does not match any of
 * the standard types), the macro 'ENV_PARA_SPECIAL' must be inserted to inform the documentation tools.
 * 'ENV_PARA_SPECIAL' may be followed by (custom) variable declaration in the same line
 * Each macro invocation must be followed by a comment with the parameter documentation in the following format:
 *
 *     ENV_PARA_... (...)
 *      \ * Short description (max. ~40 chars, no trailing period)
 *        *
 *        * Optional long description, optionally covering
 *        * multiple lines or empty lines. Only the last line must not be empty, and there must be exactly one empty
 *        * line between the short and the long description.
 *        * /
 *
 * **Note:** Persistent variables ("var.*" domain) cannot be initialized this way.
 *   They must be declared via 'ENV_PARA_SPECIAL'.
 *
 * @{
 */


enum EEnvParaType {
  eptString = 0,
  eptPath,
  eptInt,
  eptFloat,
  eptBool
};


class CEnvPara {
  public:
    CEnvPara (const char *_key, EEnvParaType _type, void *_pVar);

    static void GetAll (bool withVarKeys = true);

  protected:
    const char *key;
    EEnvParaType type;
    void *pVar;

    CEnvPara *next;
    static CEnvPara *first;
};


/// @name Variables with standard types ...
/// @{
#define ENV_PARA_STRING(key,varName,varDefault) const char *varName = varDefault; CEnvPara varName##PreInit (key, eptString, &varName); const char *varName##Key = key;
#define ENV_PARA_PATH(key,varName,varDefault) const char *varName = varDefault; CEnvPara varName##PreInit (key, eptPath, &varName); const char *varName##Key = key;
#define ENV_PARA_INT(key,varName,varDefault) int varName = varDefault; CEnvPara varName##PreInit (key, eptInt, &varName); const char *varName##Key = key;
#define ENV_PARA_FLOAT(key,varName,varDefault) float varName = varDefault; CEnvPara varName##PreInit (key, eptFloat, &varName); const char *varName##Key = key;
#define ENV_PARA_BOOL(key,varName,varDefault) bool varName = varDefault; CEnvPara varName##PreInit (key, eptBool, &varName); const char *varName##Key = key;
/// @}

/// @name Variables for special cases ...
/// such as free types, manual initialization or read-only variables.
/// @{
#define ENV_PARA_VAR(key,varType,varName,varDefault) varType varName = varDefault; const char *varName##Key = key;
/// @}

/// @name Keys without a variable ...
/// The macro just defines a key name constant variable.
/// @{
#define ENV_PARA_NOVAR(key,varType,varName,varDefault) const char *varName##Key = key;
  ///< Argument 'varType' is used for automatic documentation.
/// @}

/// @name Groups of variables ...
/// such as patterned variables like `doorman.\<phone\>.register`.
/// @{
#define ENV_PARA_SPECIAL(keyPattern, varType, varDefault)
  ///< Arguments 'varType' and 'varDefault' are used for automatic documentation.
/// @}


/// @}  // @defgroup


/// @}  // @file
#ifdef SWIG
%pythoncode %{
## @}
%}
#endif


#endif