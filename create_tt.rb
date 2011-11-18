#!/usr/bin/env ruby

require 'fileutils'

training_dir = "training-set"
test_dir = "test-set"

if File.exists?(training_dir) or File.exists?(test_dir)
  puts "training-set or test-set exist"
  exit 1
end

trset = ARGV.shift
trset = trset ? trset.to_f : 0.6
teset = ARGV.shift
teset = teset ? teset.to_f : 0.4

p [trset, teset]

Dir.mkdir training_dir
Dir.mkdir File.join(training_dir, 'img')
Dir.mkdir test_dir
Dir.mkdir File.join(test_dir, 'img')
Dir["sample*"].each do |dir|
  puts dir
  imgdir = File.join(dir, 'img')
  list = File.readlines(File.join(dir, 'list.txt')).map{|l| l =~ /(\d+)\t(\d)\t(\d)/; [$1, $2, $3]}

  lists = [
    list.select{|l| l[1] == '1' and l[2] == '1'}.sort_by{rand},
    list.select{|l| l[1] == '1' and l[2] == '0'}.sort_by{rand},
    list.select{|l| l[1] == '0' and l[2] == '1'}.sort_by{rand},
    list.select{|l| l[1] == '0' and l[2] == '0'}.sort_by{rand}
  ]

  trlist = []
  lists.each{|l| rbound = (trset*l.size).to_i; p rbound; trlist |= l[0..rbound]}
  telist = []
  lists.each{|l| lbound = (trset*l.size).to_i; rbound = lbound + (teset*l.size).to_i; p [lbound, rbound]; telist |= l[lbound..rbound]}

  [[training_dir,trlist], [test_dir,telist]].each do |dst, l|
    imgdir_dst = File.join(dst, 'img')
    list_file = File.open(File.join(dst, 'list.txt'), 'a')
    l.each do |line|
      fnumber, same, bad = line
      f1src = File.join(imgdir, "#{fnumber}-1.tif")
      f2src = File.join(imgdir, "#{fnumber}-2.tif")
      f1dst = File.join(imgdir_dst, "#{fnumber}-1.tif")
      f2dst = File.join(imgdir_dst, "#{fnumber}-2.tif")
      FileUtils.cp(f1src, f1dst)
      FileUtils.cp(f2src, f2dst)
      list_file << line.join("\t") << "\n"
    end
    list_file.close
  end
end
