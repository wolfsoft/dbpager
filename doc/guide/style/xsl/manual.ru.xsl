<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output
   method="xml"
   encoding="UTF-8"
   indent="no"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

<xsl:variable name="messages" select="document('../lang/ru.msg')/messages"/>

<xsl:variable name="output-encoding" select="'UTF-8'"/>

<xsl:param name="file-extension" select="'xml'"/>

<xsl:param name="media" select="common.xsl"/>

<xsl:include href="common.xsl"/>

</xsl:stylesheet>
