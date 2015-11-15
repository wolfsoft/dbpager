<?xml version="1.0"?>

<!DOCTYPE xsl:stylesheet SYSTEM "../dtd/common.dtd">

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://www.w3.org/1999/xhtml">
  
<xsl:variable name="metafile" select="document(/*/@metafile)/metafile"/>
<xsl:variable name="path" select="$metafile/relpath"/>
<xsl:variable name="lang" select="//@lang"/>
<xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template name="head">
	<head>
		<link href="{$path}/style/css/common.css" rel="stylesheet"
			type="text/css" media="all" title="main stylesheet"/>
		<link href="{$path}/style/css/magula.css" rel="stylesheet"
			type="text/css" media="all"/>
		<script type="text/javascript" src="{$path}/style/js/highlight.pack.js"></script>
		<script type="text/javascript">hljs.tabReplace = '    ';hljs.initHighlightingOnLoad();</script>
		<title><xsl:value-of select="title"/> - dbPager Server</title>
	</head>
</xsl:template>

<xsl:template name="top">
	<a href="http://dbpager.org.ru" rel="start">
		<img src="{$path}/images/dbpager-logo.png" align="left"
			title="{$messages/message[@name='homepage']}"/>
	</a>
	<div id="menu">
		<ul>
			<li><a href="{$path}index.{$lang}.{$file-extension}" rel="contents"><xsl:value-of select="$messages/message[@name='toc']"/></a></li>
			<li><a href="{$path}{$metafile/parentdocument}.{$lang}.{$file-extension}" rel="prev"><xsl:value-of select="$messages/message[@name='prev-chapter']"/></a></li>
			<li><a href="{$path}{$metafile/followdocument}.{$lang}.{$file-extension}" rel="next"><xsl:value-of select="$messages/message[@name='next-chapter']"/></a></li>
			<li><a href="https://sourceforge.net/apps/phpbb/dbpager/index.php" rel="help"><xsl:value-of select="$messages/message[@name='send-feedback']"/></a></li>
		</ul>
	</div>
	<p class="title"><xsl:value-of select="title"/></p>
	<p class="subtitle">
		<xsl:value-of select="$messages/message[@name='langavail']"/>:
		<xsl:for-each select="$metafile/variants/variant">
			<xsl:sort select="."/>
			<a href="{$metafile/basename}.{.}.{$file-extension}" alt="alternate">
				<xsl:value-of select="."/>
			</a>
			<xsl:if test="position() != last()">
				<xsl:text> | </xsl:text>
			</xsl:if>
		</xsl:for-each>
	</p>
</xsl:template>

<xsl:template name="bottom">
	<p class="copyright"><xsl:value-of select="$messages/message[@name='copyright']"/></p>
</xsl:template>

<xsl:template name="toc">
	<table cellpadding="4" width="100%">
		<tr>
			<td class="header_toc">
				<xsl:value-of select="$messages/message[@name='toc']"/>
			</td>
		</tr>
		<tr>
			<td class="body_toc">
				<ol>
					<xsl:for-each select="section">
						<li><a href="#{@id}" rel="chapter"><xsl:value-of select="title"/></a></li>
					</xsl:for-each>
				</ol>
			</td>
		</tr>
	</table>
</xsl:template>

<xsl:template match="/manualpage">
<html xml:lang="{$messages/@lang}" lang="{$messages/@lang}">
<xsl:call-template name="head"/>
	<body>
		<xsl:call-template name="top"/>
		<table cellpadding="4" width="100%">
			<tr>
				<td class="body_hidden" width="100%"><xsl:apply-templates/></td>
				<td><xsl:call-template name="toc"/></td>
			</tr>
		</table>
		<xsl:call-template name="bottom"/>
	</body>
</html>
</xsl:template>

<xsl:template match="glossary">
	<xsl:for-each select="item[not(substring(@name, 1, 1) = substring(following::item/@name, 1, 1))]">
		<xsl:sort select="substring(@name, 1, 1)"/>
		<a href="#glossary{substring(@name, 1, 1)}">
			<xsl:value-of select="translate(substring(@name, 1, 1), $smallcase, $uppercase)" />
		</a>
		<xsl:text>&#160;</xsl:text>
	</xsl:for-each>
	<xsl:for-each select="item[not(substring(@name, 1, 1) = substring(following::item/@name, 1, 1))]">
		<xsl:sort select="substring(@name, 1, 1)"/>
		<a id="">
			<xsl:attribute name="id">glossary<xsl:value-of select="substring(@name, 1, 1)"/></xsl:attribute>
		<h1 class="glossary">
			<span id="inverse"><xsl:value-of select="translate(substring(@name, 1, 1), $smallcase, $uppercase)" /></span>
		</h1>
		</a>
		<xsl:for-each select="../item[substring(@name, 1, 1) = substring(current()/@name, 1, 1)]">
			<xsl:sort select="@name"/>
			<p><b><xsl:value-of select="@name" /></b>. <xsl:value-of select="." /></p>
		</xsl:for-each>
	</xsl:for-each>
</xsl:template>

<xsl:template match="/indexpage">
<html xml:lang="{$messages/@lang}" lang="{$messages/@lang}">
<xsl:call-template name="head"/>
	<body>
		<xsl:call-template name="top"/>
		<xsl:apply-templates select="summary"/>
		<h1><xsl:value-of select="$messages/message[@name='toc']"/></h1>
		<table width="100%">
			<tr>
				<td width="50%">
					<xsl:for-each select="category[position() mod 2 = 1]">
						<xsl:apply-templates select="."/>
					</xsl:for-each>
				</td>
				<td width="50%">
					<xsl:for-each select="category[position() mod 2 != 1]">
						<xsl:apply-templates select="."/>
					</xsl:for-each>
				</td>
		  </tr>
		</table>
		<xsl:call-template name="bottom"/>
	</body>
</html>
</xsl:template>

<xsl:template match="category">
	<h2><xsl:value-of select="title"/></h2>
	<ul><xsl:apply-templates select="page"/></ul>
</xsl:template>

<xsl:template match="page">
	<li>
		<a href="{../@id}/{@href}.{$lang}.{$file-extension}" rel="section">
			<xsl:apply-templates/>
		</a>
	</li>
</xsl:template>

<xsl:template match="section">
	<a name="{@id}">
		<xsl:apply-templates select="title" mode="print"/>
		<p><xsl:apply-templates/></p>
	</a>
</xsl:template>

<xsl:template match="title"/>

<xsl:template match="title" mode="print">
	<a name="{@id}">
		<h1><xsl:apply-templates/></h1>
	</a>
</xsl:template>

<xsl:template match="subtitle">
	<a name="{@id}">
		<h2><xsl:apply-templates/></h2>
	</a>
</xsl:template>

<xsl:template match="summary">
	<table>
		<tr>
			<td width="8px" class="bg"></td>
			<td width="8px"></td>
			<td><i><xsl:apply-templates/></i></td>
		</tr>
	</table>
	<br/>
</xsl:template>

<xsl:template match="p">
	<p><xsl:apply-templates/></p>
</xsl:template>

<xsl:template match="link">
	<xsl:choose>
		<xsl:when test="substring-before(@to, '://') != ''">
			<a href="{@to}">
				<xsl:apply-templates/>
				<img src="../images/external.png" style="padding-left: 0.25em;"/>
			</a>
		</xsl:when>
		<xsl:otherwise>
			<a href="{@to}.{$lang}.{$file-extension}">
				<xsl:if test="@bookmark">
					<xsl:attribute name="href">
						<xsl:value-of select="@to"/>.<xsl:value-of select="$lang"/>.<xsl:value-of select="$file-extension"/>#<xsl:value-of select="@bookmark"/></xsl:attribute>
				</xsl:if>
				<xsl:apply-templates/>
			</a>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="table">
	<table cellpadding="8" cellspacing="0">
		<xsl:apply-templates select="header"/>
		<xsl:apply-templates select="row"/>
	</table>
</xsl:template>

<xsl:template match="header">
	<tr>
	  <xsl:for-each select="cell">
		  <td class="header_{../../@style}">
				<xsl:if test="position() = 1">
					<xsl:attribute name="nowrap">nowrap</xsl:attribute>
				</xsl:if>
				<b><xsl:apply-templates/></b>
			</td>
		</xsl:for-each>
	</tr>
</xsl:template>

<xsl:template match="row">
	<tr>
		<xsl:choose>
			<xsl:when test="position() = last()">
				<xsl:apply-templates select="cell" mode="last"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="cell"/>
			</xsl:otherwise>
		</xsl:choose>
	</tr>
</xsl:template>
	
<xsl:template match="cell">
	<td class="body_{../../@style}">
		<xsl:if test="position() = 1">
			<xsl:attribute name="nowrap">nowrap</xsl:attribute>
		</xsl:if>
	  <xsl:apply-templates/>
	</td>
</xsl:template>

<xsl:template match="cell" mode="last">
	<td class="footer_{../../@style}">
		<xsl:if test="position() = 1">
			<xsl:attribute name="nowrap">nowrap</xsl:attribute>
		</xsl:if>
	  <xsl:apply-templates/>
	</td>
</xsl:template>

<xsl:template match="term">
	<b><xsl:apply-templates/></b>
</xsl:template>

<xsl:template match="cmd">
	<pre><code class="no-highlight"><xsl:apply-templates/></code></pre>
</xsl:template>	

<xsl:template match="name">
	<span class="name"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="param">
	<span class="param"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="value">
	<span class="value"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="code">
	<table width="100%" cellpadding="8">
		<tr><td class="bg2"><pre><code><xsl:if test="@highlight='off'"><xsl:attribute name="class">no-highlight</xsl:attribute></xsl:if><xsl:apply-templates/></code></pre></td></tr>
	</table>
</xsl:template>

<xsl:template match="example">
	<table width="100%" cellpadding="0" cellspacing="0">
		<tr>
			<td width="4px" class="bg"></td>
			<td class="bg">
				<img src="{$path}images/example.png"
					alt="{$messages/message[@name='example']}"/>
			</td>
			<td class="bg">
				<h2 class="block">
					<xsl:value-of select="$messages/message[@name='example']"/>:&nbsp;
					<xsl:value-of select="title"/>
				</h2>
			</td>
		</tr>
		<tr>
			<td width="8px" class="bg"></td>
			<td width="4px"></td>
			<td>
				<xsl:apply-templates select="comment"/>
				<h3><xsl:value-of select="$messages/message[@name='source']"/></h3>
				<xsl:apply-templates select="input"/>
				<h3><xsl:value-of select="$messages/message[@name='result']"/></h3>
				<xsl:apply-templates select="output"/>
				<br/>
			</td>
		</tr>
	</table>
</xsl:template>

<xsl:template match="example/input">
	<pre><code><xsl:apply-templates/></code></pre>
</xsl:template>

<xsl:template match="example/output">
	<pre><code><xsl:apply-templates/></code></pre>
</xsl:template>

<xsl:template match="example/comment">
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="tip">
	<table width="100%" cellpadding="8" class="tip">
		<tr>
			<td>
				<img src="{$path}images/tip.png"
					alt="{$messages/message[@name='tip']}"/>
			</td>
			<td width="100%">
				<h4><xsl:value-of select="$messages/message[@name='tip']"/></h4>
				<xsl:apply-templates/>
			</td>
	  </tr>
	</table>
</xsl:template>

<xsl:template match="note">
	<table width="100%" cellpadding="8" class="note">
		<tr>
			<td>
				<img src="{$path}images/note.png"
					alt="{$messages/message[@name='note']}"/>
			</td>
			<td width="100%">
				<h4><xsl:value-of select="$messages/message[@name='note']"/></h4>
				<xsl:apply-templates/>
			</td>
	  </tr>
	</table>
</xsl:template>

<xsl:template match="caution">
	<table width="100%" cellpadding="8" class="caution">
		<tr>
			<td>
				<img src="{$path}images/caution.png"
					alt="{$messages/message[@name='caution']}"/>
			</td>
			<td width="100%">
				<h4><xsl:value-of select="$messages/message[@name='caution']"/></h4>
				<xsl:apply-templates/>
			</td>
	  </tr>
	</table>
</xsl:template>

<xsl:template match="index">
	<xsl:for-each select="link">
		<xsl:sort select="."/>
		<p><xsl:apply-templates select="."/></p>
	</xsl:for-each>
</xsl:template>

<xsl:template match="list">
	<ol>
		<xsl:for-each select="item">
			<li><xsl:apply-templates/></li>
		</xsl:for-each>
	</ol>
</xsl:template>

<xsl:template match="image">
	<img src="{concat($path,'/images/', @src)}"/>
</xsl:template>

<xsl:template match="graph">
	<xsl:copy-of select="document(concat($path,'../images/', @src))"/>
</xsl:template>

</xsl:stylesheet>
