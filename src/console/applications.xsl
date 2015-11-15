<?xml version="1.0"?>

<!DOCTYPE xsl:stylesheet SYSTEM "dtd/xhtml1-strict.dtd">

<xsl:stylesheet version="1.0"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns="http://www.w3.org/1999/xhtml">

<xsl:template match="applications">
	<script type="text/javascript" src="js/manage-applications.js"></script>
	<div class="group">
	<span class="group-title"><xsl:value-of select="$messages/message[@name='applications']"/></span><br/>
	<input type="button" name="install" value="{$messages/message[@name='install-new-application']}"/><br/>
	<xsl:for-each select="item">
		<div style="padding: 8px; clear: both;">
			<img style="float: left;" width="32" height="32" src="images/databases.png" alt="dbPager Server Console"/>
			<div style="float: left; width: 70%;">
				<h3><xsl:value-of select="name"/></h3>
				<p class="desc">Version: <xsl:value-of select="version"/></p>
				<p class="desc"><xsl:value-of select="description"/></p>
			</div>
			<div style="float: left; margin-left: 4px;">
				<input type="button" style="margin: 1px; width: 8em;" name="update" value="Update..."/>
				<input type="button" style="margin: 1px; width: 8em;" name="stop" value="Stop..."/><br/>
				<input type="button" style="margin: 1px; width: 8em;" name="setup" value="Setup..."/>
				<input type="button" style="margin: 1px; width: 8em;" name="backup" value="Backup..."/><br/>
				<input type="button" style="margin: 1px; width: 8em;" name="uninstall" value="Uninstall..."/>
				<input type="button" style="margin: 1px; width: 8em;" name="restore" value="Restore..."/>
			</div>
		</div>
	</xsl:for-each>
	</div>
</xsl:template>

</xsl:stylesheet>
