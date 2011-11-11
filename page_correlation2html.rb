#!/usr/bin/env ruby

def help
  puts "page_correlation2html <log file|-> <out_dir>"
end

if ARGV.size != 2
  help
  exit
end

log=ARGV.shift
outdir=ARGV.shift

if log=="-"
  $l = STDIN
else
  $l = File.open log
end

Dir.mkdir outdir unless File.exists?(outdir)
unless File.directory? outdir
  help
  exit
end

$imgdir = File.join(outdir, "img")
Dir.mkdir $imgdir unless File.directory?($imgdir)

$index = File.open File.join(outdir, "index.html"), "w"
$index.sync = true
$index << %~
<html>
<head>
  <title>page_correlation</title>
  <style type="text/css">
    * {
      font-size: 10pt;
    }
    td, th {
      text-align: center; 
    }
    .different {
      background: red;
    }
    .same {
      background: green;
    }
    .violation {
      font-weight: bold;
      color: yellow;
    }
    #filter {
      background: black;
      color: lightgray;
      width: 100%;
    }
    #content {
      padding: 1em;
    }
  </style>
  <script type="text/javascript">
    function filterLines() {
      var selected = new Array();
      var inputs = document.getElementsByName("line");
      for (var i=0; i<inputs.length; i++) {
        if (inputs[i].checked) {
          selected.push(i);
        }
      }

      var out = document.getElementById("content");
      var logformat = document.getElementById("logformat");
      var f1 = document.getElementById("f1");
      var f2 = document.getElementById("f2");
      var tmp = "";

      if (logformat.value == "full") {
        for (var i=0; i<selected.length; i++) {
          tmp = tmp + lines[i] + "<br />";
        }
      } else {
        var f1pattern = /^(.*) \\([^\\)]+\\) - .* \\([^\\)]+\\).*/
        var f2pattern = /^.* \\([^\\)]+\\) - (.*) \\([^\\)]+\\).*/
        for (var i=0; i<selected.length; i++) {
          var attrs = new Array();
          if (f1.checked) {
            attrs.push(f1pattern.exec(lines[i])[1]);
          }
          if (f2.checked) {
            attrs.push(f2pattern.exec(lines[i])[1]);
          }
          tmp = tmp + attrs.join("&nbsp;&nbsp;&nbsp;") + "<br />";
        }
      }

      out.innerHTML = tmp;
    }

    function toggle(node) {
      var td = node.parentNode;
      var ns = document.getElementById("num_selected");
      if (/blue/.test(td.style.background)) {
        td.style.background = "transparent";
        --num_selected;
      } else {
        td.style.background = "blue";
        ++num_selected;
      }
      ns.innerHTML = "" + num_selected;
    }

    function updateSelected() {
      var ns = document.getElementById("num_selected");
      var checkboxes = document.getElementsByName("line");
      for (var i=0; i<checkboxes.length; i++) {
        if (checkboxes[i].checked) {
          toggle(checkboxes[i]);
        }
      }
      ns.innerHTML = "" + num_selected;
    }
  </script>
</head>
<body onLoad="updateSelected()">

<script type="text/javascript">
    var lines = new Array();
    var num_selected = 0;
</script>

<form>
  <table>
    <tr>
      <th></th>
      <th></th>
      <th></th>
      <th>Page 1</th>
      <th>Page 2</th>
      <th>Size ratio</th>
      <th>Same</th>
      <th>Correlation</th>
      <th>Decision</th>
    </tr>
~

TSIZE=160
RE=%r{^(.*) \(([^\)]+)\) - (.*) \(([^\)]+\))   Size ratio: (.*)   numaa: (.*)   min_numa: (.*)   same: (\d)   Correlation: (.*)   Decision: (\d)$}
i=0
num_same = num_diff = num_sizes = 0
while not $l.eof?
  line=$l.readline
  m = RE.match(line)
  next unless m
  f1, res1, f2, res2, size_ratio, numaa, min_numa, same, correlation, decision = m[1..-1]
  fdir = File.dirname(f1)
  issue = File.dirname(fdir)
  File.basename(f1) =~ /^(\d+)/
  fnum = $1
  i1 = "%06d-1.jpg" % i
  i2 = "%06d-2.jpg" % i
  `convert "#{f1}" -thumbnail #{TSIZE}x "#{$imgdir}/#{i1}"` unless File.exists?("#{$imgdir}/#{i1}")
  `convert "#{f2}" -thumbnail #{TSIZE}x "#{$imgdir}/#{i2}"` unless File.exists?("#{$imgdir}/#{i2}")
  decision.to_i == 0 ? num_diff+=1 : num_same+=1
  num_sizes += 1 if size_ratio.to_f<=0.6
  $index << %~
    <script type="text/javascript">
      lines.push("#{line.strip}");
    </script>
    <tr class="#{decision.to_i==1 ? 'same' : 'different'}">
      <td><input type="checkbox" name="line" value="#{i}" onClick="toggle(this)" /></td>
      <td><img src="img/#{i1}" /></td>
      <td><img src="img/#{i2}" /></td>
      <td>#{f1} (#{res1})</td>
      <td>#{f2} (#{res2})</td>
      <td #{size_ratio.to_f<=0.6 ? 'class="violation"' : ""}>#{size_ratio}</td>
      <td>#{same}</td>
      <td>#{correlation}</td>
      <td>#{decision}</td>
    </tr>
  ~

  i+=1
end

$index << %~
  </table>
  <p>
    <b>Total:</b> #{i} <b>Same:</b> #{num_same} <b>Different:</b> #{num_diff} <b>Size:</b> #{num_sizes}
  </p>
  <div>
    <p>Filter out logs for <span id="num_selected"></span> selected images:</p>
    Log format:
    <select id='logformat' name='logformat'>
      <option value="full">Full log</option>
      <option value="attributes">Selected Attributes</option>
    </select><br />
    Page1 file name <input type="checkbox" name="f1" id="f1" checked="checked" /> <br />
    Page2 file name <input type="checkbox" name="f1" id="f2" /><br />
    <br />
    <input type="submit" name="filter" value="Filter" onClick="filterLines(); return false;" />
  </div>
  <br />
  <div id="filter"><div id="content"></div></div>
</form>
</body>
~

$index.close
$l.close if $l != STDIN
