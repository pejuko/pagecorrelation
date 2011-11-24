#!/usr/bin/env ruby

require 'fileutils'

IMAGE_SIZE=200

training_dir = "set-train"
test_dir = "set-test"
cross_dir= "set-cross"


if File.exists?(training_dir) or File.exists?(test_dir) or File.exists?(cross_dir)
  puts "training-set or test-set exist"
  exit 1
end

trset = ARGV.shift
trset = trset ? trset.to_f : 0.5
crset = ARGV.shift
crset = crset ? crset.to_f : 0.3
teset = ARGV.shift
teset = teset ? teset.to_f : 0.2

p [trset, crset, teset]

[training_dir, test_dir, cross_dir].each do |dir|
  Dir.mkdir dir
  Dir.mkdir File.join(dir, 'img')
end

Dir["sample*"].each do |dir|
  puts dir
  imgdir = File.join(dir, 'img')
  list = File.readlines(File.join(dir, 'list.txt')).map{|l| l =~ /(\d+)\t(\d)\t(\d)/; [$1, $2, $3]}

  lists = [
#    list.select{|l| l[1] == '1' and l[2] == '1'}.sort_by{rand},
    list.select{|l| l[1] == '1' and l[2] == '0'}.sort_by{rand},
#    list.select{|l| l[1] == '0' and l[2] == '1'}.sort_by{rand},
    list.select{|l| l[1] == '0' and l[2] == '0'}.sort_by{rand}
  ]

  trlist = []
  lists.each{|l| rbound = (trset*l.size).to_i; p rbound; trlist |= l[0..rbound]}

  telist = []
  lists.each{|l| lbound = (trset*l.size).to_i; rbound = lbound + (teset*l.size).to_i; p [lbound, rbound]; telist |= l[lbound..rbound]}

  crlist = []
  lists.each{|l| lbound = (trset*l.size).to_i + (teset*l.size).to_i; rbound = lbound + (crset*l.size).to_i; p [lbound, rbound]; crlist |= l[lbound..rbound]}

  [[training_dir,trlist], [cross_dir, crlist], [test_dir,telist]].each do |dst, l|
    imgdir_dst = File.join(dst, 'img')
    list_file = File.open(File.join(dst, 'list.txt'), 'a')
    l.each do |line|
      fnumber, same, bad = line
      f1src = File.join(imgdir, "#{fnumber}-1.tif")
      f2src = File.join(imgdir, "#{fnumber}-2.tif")
      f1dst = File.join(imgdir_dst, "#{fnumber}-1.tif")
      f2dst = File.join(imgdir_dst, "#{fnumber}-2.tif")
      #FileUtils.cp(f1src, f1dst)
      #FileUtils.cp(f2src, f2dst)
      `gm convert #{f1src} -thumbnail #{IMAGE_SIZE}x#{IMAGE_SIZE}! -colorspace Gray -depth 8 #{f1dst}`
      `gm convert #{f2src} -thumbnail #{IMAGE_SIZE}x#{IMAGE_SIZE}! -colorspace Gray -depth 8 #{f2dst}`
      list_file << line.join("\t") << "\n"
    end
    lb = l.select{|line| line[1] == '0'}
    lg = l.select{|line| line[1] == '1'}
    if lb.size < lg.size
      n = (lg.size - lb.size)
      h = n/2
      n.times do |i|
        i2 = (i+h) % (lg.size-1)
        f1src = File.join(imgdir, "#{lg[i][0]}-1.tif")
        f2src = File.join(imgdir, "#{lg[i2][0]}-2.tif")
        f1dst = File.join(imgdir_dst, "#{lg[i][0]}#{lg[i2][0]}-1.tif")
        f2dst = File.join(imgdir_dst, "#{lg[i][0]}#{lg[i2][0]}-2.tif")
        `gm convert #{f1src} -thumbnail #{IMAGE_SIZE}x#{IMAGE_SIZE}! -colorspace Gray -depth 8 #{f1dst}`
        `gm convert #{f2src} -thumbnail #{IMAGE_SIZE}x#{IMAGE_SIZE}! -colorspace Gray -depth 8 #{f2dst}`
        line = "#{lg[i][0]}#{lg[i2][0]}\t0\t0"
        list_file << line << "\n"
      end
    end
    list_file.close
  end
end
