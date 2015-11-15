<?xml version="1.0"?>

<!DOCTYPE xsl:stylesheet SYSTEM "dtd/xhtml1-strict.dtd">

<xsl:stylesheet version="1.0"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns="http://www.w3.org/1999/xhtml">

<xsl:output
   method="html"
   encoding="utf-8"
   indent="no"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

<xsl:variable name="lang" select="//@lang"/>
<xsl:variable name="lang-file" select="concat('lang/',$lang,'.msg')"/>
<xsl:variable name="messages" select="document($lang-file)/messages"/>

<xsl:include href="applications.xsl"/>

<xsl:template match="*|@*">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()"/>
	</xsl:copy>
</xsl:template>

<xsl:template match='text()'><xsl:value-of select='.'/></xsl:template>

<xsl:template match="menu">
	<ul id="menu">
		<xsl:for-each select="item">
			<li><a href="{@link}" title="{text()}"><xsl:value-of select="@title"/></a></li>
		</xsl:for-each>
		<li>&#160;</li>
	</ul>
</xsl:template>

<xsl:template match="status">
	<ul id="status">
		<xsl:for-each select="item">
			<li><xsl:value-of select="@title"/>: <span class="name"><xsl:value-of select="text()"/></span></li>
		</xsl:for-each>
	</ul>
</xsl:template>

<xsl:template match="block">
	<div class="block">
		<p class="title"><xsl:value-of select="@title"/></p>
		<div class="body">
			<xsl:apply-templates/>
		</div>
	</div>
</xsl:template>

<xsl:template match="tasks">
	<div class="group">
	<span class="group-title"><xsl:value-of select="$messages/message[@name='tasks']"/></span><br/>
	<xsl:for-each select="group">
		<h3><xsl:value-of select="@title"/></h3>
		<xsl:for-each select="item">
			<div class="group-element">
				<img style="float: left;" width="32" height="32" src="{@icon}" alt="{@title}"/>
				<div style="margin-left: 40px;">
					<p><a href="{@href}"><xsl:value-of select="@title"/></a></p>
					<p class="desc"><xsl:value-of select="text()"/></p>
				</div>
			</div>
		</xsl:for-each>
	</xsl:for-each>
	</div>
</xsl:template>

<xsl:template match="copyright">
	<div id="footer">
		<p><xsl:value-of select="text()"/></p>
	</div>
</xsl:template>

<xsl:template match="text">
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="page">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="{@lang}" lang="{@lang}">
	<head>
		<title>dbPager Server Console</title>
		<script type="text/javascript" src="js/jquery-1.3.2.min.js"></script>
		<script type="text/javascript" src="js/jquery.timers-1.2.0.js"></script>
		<script type="text/javascript" src="js/update-statistics.js"></script>
		<link href="css/console.css" rel="stylesheet"
		  type="text/css" media="all" title="main stylesheet"/>
	</head>
	<body>
		<div id="title">
			<p>dbPager Server Console</p>
		</div>
		<div id="menu-line">
			<xsl:apply-templates select="menu"/>
			<xsl:apply-templates select="status"/>
		</div>
		<div id="summary">
			<xsl:apply-templates select="block"/>
		</div>
		<div id="main">
			<xsl:apply-templates select="text"/>
			<xsl:apply-templates select="tasks"/>
			<xsl:apply-templates select="applications"/>
		</div>
		<xsl:apply-templates select="copyright"/>
	</body>
</html>
</xsl:template>

</xsl:stylesheet>
