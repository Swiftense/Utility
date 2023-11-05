<h1>Swiftense Utility Library</h1>
A basic utility Library that is used across the Project and contains a variety of different Algorithms.
<h2>List</h2>
<ul>
<li><strong>Text Parser</strong> (Utils/StringIterator/StringIterator.h)</li>
<li><strong>Json Parser</strong> (Utils/StringIterator/StringIterator.h)</li>
<li><strong>Hashmap Algorithm</strong> (Utils/SHashmap.h)</li>
<li><strong>Quick Search Tree Algorithm</strong> (Utils/SQTree.h)</li>
<li><strong>Dynamic String Utility</strong> (Utils/String/String.h)</li>
<li><strong>Number Conversion & Base64 Utility</strong> (Utils/String/Convert.h)</li>
<li><strong>Vector Utility</strong> (Utils/Vector.h)</li>
</ul>

<h1>Json Parser</h1>
<h2>Performance</h2>

Read/Write Performance test on 100K lines of json data

![](benchmark.png)

<h2>Usage</h2>
<ul>
<li>1.) Create Iterator using <strong>itr_loadFromLargeFile</strong> Function</li>
<li>2.) Parse Data using <strong>itr_getAbstract</strong> Function</li>
<li>3.) Modify Data</li>
<li>4.) Dump or Stringify updated Data using <strong>itr_dump</strong> or <strong>itr_stringify</strong> Functions</li>
</ul>

<h1>License</h1>
<p xmlns:cc="http://creativecommons.org/ns#" >This work is licensed under <a href="http://creativecommons.org/licenses/by-nc-nd/4.0/?ref=chooser-v1" target="_blank" rel="license noopener noreferrer" style="display:inline-block;">Attribution-NonCommercial-NoDerivatives 4.0 International<img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/nc.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/nd.svg?ref=chooser-v1"></a></p>