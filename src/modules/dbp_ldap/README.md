# dbPager LDAP plugin

dbPager provides a plugin to connect to the LDAP directory server, allowing authentication of users and queries from that database. To do this, the plugin provides three functions:

- `ldap:connection` specifies the URL of the LDAP server without a database connection
- `ldap:bind` connects to the database and does authentication
- `ldap:search` searches for data in the database

## Using the plugin

The plugin does not require any settings in configuration files. The configuration file `/etc/dbpager/modules-enabled/dbp_ldap.conf` defines the namespace used by plugin functions:

`http://dbpager.org.ru/schemas/dbp/3.0/ldap`

To use the plugin functions, you need to define this namespace in the dbPager `dbp:module` tag:

```xml
<?xml version="1.0"?>

<dbp:module
	xmlns:dbp="http://dbpager.org.ru/schemas/dbp/3.0"
	xmlns:ldap="http://dbpager.org.ru/schemas/dbp/3.0/ldap"
>

(you can use LDAP functions here now)

</dbp:module>
```

## Functions reference

### ldap:connection

The `ldap:connection` function defines the LDAP server URL to work with. If the URL is correct, it defines the LDAP server context, executes all nested tags, or raises the exception otherwise.

#### Parameters

- `href` the URL of the LDAP server, i.e. `ldap://localhost:1389`

#### Example

```xml
<dbp:try>
    <ldap:connection href="ldap://localhost:1389">
        (do any other LDAP operations here)
    </ldap:connection>

    <dbp:catch>
        The LDAP server URL is incorrect: {$error_message}
    </dbp:catch>
</dbp:try>
```

### ldap:bind

The `ldap:bind` function performs password authentication against the LDAP server. If authentication is successful, all nested tags are executed, otherwise an exception is raised.

#### Parameters

- `dn` the distinguished name of the user for authentication. This is like a username in LDAP terms. For example, `cn=admin,dc=example,dc=org`
- `password` the user password, i.e. `adminpassword`

#### Example

```xml
<ldap:connection href="ldap://localhost:1389">

    <dbp:try>

        <ldap:bind dn="cn=admin,dc=example,dc=org" password="adminpassword">
            (do any LDAP searches here)
        </ldap:bind>

        <dbp:catch>
            The user name or the password are incorrect.
        </dbp:catch>
    </dbp:try>

</ldap:connection>
```

### ldap:search

The `ldap:search` function queries the LDAP server for data on an LDAP subtree using a filter. If the server returns one or more entries, the function initializes variables to the entry's attribute name and value and executes nested tags. If the server returns multiple entries, the function loops through each entry. If there are no entries, the nested tags are not executed. If the server returns an error, an exception is raised.

The request is executed in the context of the currently authenticated user.

#### Parameters

- `base` The LDAP search base, the starting point for the search, i.e. `ou=users,dc=example,dc=org`
- `filter` The LDAP filter string, i.e. `(uid=user1)`

#### Example

```xml
<ldap:bind dn="cn=admin,dc=example,dc=org" password="adminpassword">

    <dbp:try>

        <ldap:search base="ou=users,dc=example,dc=org" filter="(uid=user1)">
            The email address of user1 is: {$email}
        </ldap:search>

        <dbp:catch>
            Perhaps you don't have access rights to access users list, LDAP server error: {$error_message}
        </dbp:catch>
    </dbp:try>

</ldap:bind>
```

## Complete code example

The complete code example shows a typical authentication flow, including two binds to the LDAP directory. The first bind is performed by the admin user and is needed to look up the DN of a user by username or email address, and the second bind is used to authenticate the found user by password.

You can run this script from the command line or using a web browser. We assume that the `username` and `password` variables will be passed to this script via command line parameters or via a web form:

```bash
dbpager example.dbpx username=user1 password=password1
```

We also assume that the LDAP server is running locally with the predefined demo schema with `docker`:

```
docker run -ti --rm --name openldap \
  -p 1389:1389 \
  --env LDAP_ADD_SCHEMAS=yes \
  --env LDAP_ADMIN_USERNAME=admin \
  --env LDAP_ADMIN_PASSWORD=adminpassword \
  --env LDAP_USERS=user1,user2 \
  --env LDAP_PASSWORDS=password1,password2 \
  --env LDAP_ROOT=dc=example,dc=org \
  --env LDAP_ADMIN_DN=cn=admin,dc=example,dc=org \
  bitnami/openldap:latest
```

example.dbpx:
```xml
<?xml version="1.0"?>

<!-- enable LDAP plugin -->
<dbp:module
	xmlns:dbp="http://dbpager.org.ru/schemas/dbp/3.0"
	xmlns:ldap="http://dbpager.org.ru/schemas/dbp/3.0/ldap"
>

<!-- define constants -->
<dbp:var name="url" value="ldap://localhost:1389" type="global" />
<dbp:var name="users_dn" value="ou=users,dc=example,dc=org" type="global" />
<dbp:var name="admin_user" value="cn=admin,dc=example,dc=org" type="global" />
<dbp:var name="admin_pass" value="adminpassword" type="global" />


<dbp:log>trying to authenticate {$username} with password {$password}...</dbp:log>

<!-- connect to the LDAP server and do authentication -->
<ldap:connection href="{$url}">
	<!-- authenticate as admin first -->
	<ldap:bind dn="{$admin_user}" password="{$admin_pass}">
		<!-- search of user DN by common name (CN), user id (UID), or email -->
		<ldap:search base="{$users_dn}" filter="(&amp;(ObjectClass=inetorgperson)(|(cn={$username})(uid={$username})(email={$username})))">
			<!-- if user found, try to authenticate it -->
			<ldap:bind dn="{$dn}" password="{$password}">
				<dbp:log>{$username} logged in.</dbp:log>
			</ldap:bind>
		</ldap:search>
	</ldap:bind>
</ldap:connection>

</dbp:module>
```

The program prints "user1 logged in" if you provide a correct password, or fails with "invalid credentials" message otherwise. If user is not found, the program prints nothing.