#!/usr/bin/env ruby

samples = sg = sb = dg = db = 0
dirs = Dir["samples*"] + ["training-set", "test-set"]
template = "%12s %8s %5s %5s %5s %5s"
head = template % %w(dir samples 10 11 00 01)
hr = "-" * head.size
puts head
puts hr
dirs.each do |dir|
  next unless File.exists?(dir)
  list = File.readlines(File.join(dir, 'list.txt')).map{|l| l =~ /(\d+)\t(\d)\t(\d)/; [$1, $2.to_i, $3.to_i]}
  dsg = dsb = ddg = ddb = 0
  list.each do |l|
    dsg += 1 if l[1]==1 and l[2]==0
    dsb += 1 if l[1]==1 and l[2]==1
    ddg += 1 if l[1]==0 and l[2]==0
    ddb += 1 if l[1]==0 and l[2]==1
  end
  puts template % [dir, list.size, dsg, dsb, ddg, ddb]
  sg += dsg
  sb += dsb
  dg += ddg
  db += ddb
  samples += list.size
end
puts hr
puts template % ["total", samples, sg, sb, dg, db]
