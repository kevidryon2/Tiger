<?php

echo "
<html>
	<head>
		<script></script>
        <style>
        	.page {
				background-color: gainsboro;
                border: 5px groove lightgray;
                width: 850px;
                margin-left: auto;
                margin-right: auto;
                padding-bottom: 0px;
    	    }
            
            .header {
            	background-color: rebeccapurple;
                width: 97.2%;
                height: 70px;
                padding-left: 21px;
            }
            
            .content {
            	background-color: white;
                border: 3px inset gainsboro;
                padding-left: 10px;
                padding-top: 0px;
                padding-bottom: 20px;
            }
            
            .logo {
            	color: white;
                margin-top: 0px;
                padding: 5px;
                font-size: 48px;
            }
            
            body {
            	font-family: sans-serif;
                background-color: indigo;
            }
            
            code {
            	font-size: 1em;
                background-color:gainsboro;
                border: 2px solid silver;
                padding-top: 1px;
                padding-bottom: 1px;
                padding-left: 3px;
                padding-right: 3px;
            }
            
            .text {
            	margin-bottom: 15px;
                margin-top: 0px;
            }
            
            .limited {
            	width: 75%;
            }
        </style>
	</head>
	<body>
    	<div class='page'>
			<div class='header'>
            	<p class='logo'><strong>Tiger</strong></p>
            </div>
            <div class='content'>
            	<h1 style='margin-bottom:5px;margin-top:10px;'>It works!</h1>
                <p class='text'>Congratulations! You managed to set up Tiger successfully.<br>
                This page is the default webpage; it's displayed since you haven't put anything
                in the <code>public</code> directory.</p>
                
                <p>
                Tiger is confined to a single directory where it stores everything about itself,
                called the Tiger root; thus, it makes it easy to, for example, serve different
                content over multiple ports using Tiger.
                </p>
                
                <p class='text'>Tiger's root contains four main directories:</p>
                <ul class='limited'>
                	<p>- <code>public</code>: contains the files that people who visit your
                    website can access, such as the <code>index.html</code> file.<br></p>
                    
                	<p>- <code>scripts</code>: contains scripts that Tiger will run when
                    someone requests a certain path or a file with a certain type; however,
                    Tiger will only run compiled scripts, so you need to use the <code>
                    netc</code> utility to compile any scripts you want to use.<br></p>
                    
                    <p>- <code>bin</code>: contains executables used by your website;
                    in this directory you can find Tiger itself, and <code>netc</code>.<br></p>
                    
                    <p>- <code>cache</code>: should be mounted to a <code>ramfs</code>;
                    it contains a cache where Tiger stores files to avoid having to reload them
                    again.<br></p>
                </ul>
            </div>
            <image src='https://www.gnu.org/graphics/agplv3-155x51.png' style='margin:5px;'></image>
        </div>
    </body>
</html>
"

?>
