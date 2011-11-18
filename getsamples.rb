#!/usr/bin/env ruby

require 'fileutils'


if ARGV.size != 6
  puts "getsamples <start number> <diff_list.txt> <bad_list.txt> <dest_dir> <file1 regexp> <file2 template>"
  puts "example:"
  puts "\tgetsamples 354 diffs-f1.txt bads-f2.txt samples/ 'source-(\d+).tif' 'enhanced-\\1.tif'"
  puts " File 1 regexp must contain '(...)' pattern and File 2 template must have \\1"
  exit 1
end

start_number = ARGV.shift.to_i
diff_list = File.readlines(ARGV.shift).map{|l| l.strip}
bad_list  = File.readlines(ARGV.shift).map{|l| l.strip}
dest_dir = ARGV.shift
f1reg = Regexp.new(ARGV.shift.strip)
f2template = ARGV.shift.strip

Dir.mkdir(dest_dir) unless File.exists?(dest_dir)
imgdir = File.join(dest_dir, 'img')
Dir.mkdir(imgdir) unless File.exists?(imgdir)
list_file = File.join(dest_dir, 'list.txt')

#i = File.exists?(list_file) ? File.readlines(list_file).size : 0
i = start_number
list = File.open(list_file, "a")
list.sync = true
while not STDIN.eof?
  line = STDIN.readline.strip
  next unless line =~ f1reg
  enhanced = line.sub(f1reg, f2template)
  next unless File.exists?(enhanced)

  puts "%04d #{line} #{enhanced}" % i

  is_same = diff_list.include?(line) ? 0 : 1
  is_bad = bad_list.include?(enhanced) ? 1 : 0

  istr = "%05d" % i
  list << "#{istr}\t#{is_same}\t#{is_bad}\n"

  f1 = File.join(imgdir, "#{istr}-1.tif")
  `gm convert #{line} -compress LZW #{f1}`
#  FileUtils.cp(line, File.join(imgdir, "#{istr}-1.tif"))
  f2 = File.join(imgdir, "#{istr}-2.tif")
  `gm convert #{enhanced} -compress LZW #{f2}`
#  `pdfimages -f 1 -l 1 #{pdf} #{f2}`
#  `gm convert #{f2}-*.p* -compress LZW #{f2}.tif`
#  `rm #{f2}-*.p*`

  i += 1
end
list.close
