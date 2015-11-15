<?xml version="1.0"?>
<!--
    manpage.xsl
    This file is part of dbPager Server

    Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>

    dbPager Server is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 3.

    dbPager Server is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with dbPager Server; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, 
    Boston, MA  02110-1301  USA
-->
<!DOCTYPE xsl:stylesheet [
	<!ENTITY mdash "\-">
]>

<xsl:stylesheet version="1.0" xml:space="preserve"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output	method="text" encoding="UTF-8" indent="no"/>

<xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<!-- man page -->

<xsl:template match="manpage">
.TH <xsl:value-of select="translate(name, $smallcase, $uppercase)"/> <xsl:value-of select="@section"/> "<xsl:value-of select="date"/>" "<xsl:value-of select="package"/>" "<xsl:value-of select="package"/>"
.\" Copyright (c) <xsl:value-of select="substring(date, 1, 4)"/> <xsl:value-of select="author/name"/> &lt;<xsl:value-of select="author/email"/>&gt;
.SH NAME
<xsl:value-of select="name"/> \- <xsl:value-of select="summary"/>
<xsl:apply-templates select="sections"/>
.SH AUTHOR
<xsl:value-of select="author/name"/> &lt;<xsl:value-of select="author/email"/>&gt;
</xsl:template>

<!-- common sections -->

<xsl:template match="synopsis">
.SH SYNOPSIS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="description">
.SH DESCRIPTION
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="options">
.SH OPTIONS
<xsl:for-each select="option">
.TP
.B <xsl:value-of select="@name"/>
<xsl:value-of select="."/>
</xsl:for-each>
</xsl:template>

<xsl:template match="files">
.SH FILES
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="environment">
.SH ENVIRONMENT
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="diagnostics">
.SH DIAGNOSTICS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="bugs">
.SH BUGS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="see-also">
.SH SEE ALSO
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="return-values">
.SH RETURN VALUES
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="examples">
.SH EXAMPLES
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="diagnostics">
.SH DIAGNOSTICS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="errors">
.SH ERRORS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="standards">
.SH STANDARDS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="security-considerations">
.SH SECURITY CONSIDERATIONS
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="other">
.SH <xsl:value-of select="translate(@name, $smallcase, $uppercase)"/>
<xsl:apply-templates/>
</xsl:template>

<!-- fonts -->

<xsl:template match="b">\fB<xsl:apply-templates/>\f1</xsl:template>

<xsl:template match="i">\fI<xsl:apply-templates/>\f1</xsl:template>

<!-- paragraphs -->

<xsl:template match="p">
.PP
<xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>

