#!/usr/bin/env ruby

samples = sg = sb = dg = db = 0
dirs = Dir["samples*"] | Dir["set-*"]
template = "%12s %8s %5s (%6.2f%%) %5s (%6.2f%%) %5s (%6.2f%%) %5s (%6.2f%%)"
head = template % ['dir', 'samples', '10', 100, '11', 100, '00', 100, '01', 100]
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
  puts template % [dir, list.size, dsg, 100*dsg.to_f/list.size, dsb, 100*dsb.to_f/list.size, ddg, 100*ddg.to_f/list.size, ddb, 100*ddb.to_f/list.size]
  sg += dsg
  sb += dsb
  dg += ddg
  db += ddb
  samples += list.size
end
puts hr
puts template % ["total", samples, sg, 100*sg.to_f/samples, sb, 100*sb.to_f/samples, dg, 100*dg.to_f/samples, db, 100*db.to_f/samples]
