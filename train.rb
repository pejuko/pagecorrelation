#!/usr/bin/env ruby

require "open3"

train_dir = "set-train"
img_dir = File.join train_dir, "img"
list_file = File.join train_dir, "list.txt"

alpha = 0.1
alpha = ARGV[0].to_f if ARGV[0]
gamma = 0.0
gamma = ARGV[1].to_f if ARGV[1]
thresh = 0.5
thresh = ARGV[2].to_f if ARGV[2]
p [alpha, gamma, thresh]

def threshold t, result
  result.map{|r| r>=t ? 1 : 0}
end

train_set = File.readlines(list_file).map{|l| l=~/(\d+)\t(\d)\t(\d)/; [l, $1, $2, $3]}
Open3.popen3("./nn_learn #{alpha} #{gamma} #{img_dir}") {|i,o,e,t|
  train_set.each do |row|
    i << row[0]
    puts o.gets
  end
}

errors = []
Open3.popen3("./nn_compare -") {|i,o,e,t|
train_set.each do |row|
  f1 = File.join img_dir, "#{row[1]}-1.tif"
  f2 = File.join img_dir, "#{row[1]}-2.tif"
  i << "#{f1} #{f2}\n"
  result = o.gets.to_s.strip
  result =~ /(\S+)   (\S+)$/
  puts "#{row[1]} [#{row[2]}, #{row[3]}] #{threshold(thresh, [$1.to_f, $2.to_f]).inspect} (#{$1}, #{$2})"
  err = [row[2].to_f - $1.to_f, row[3].to_f - $2.to_f]
  errors << err
end
}

err = errors.inject([0.0,0.0]) {|sum,e| sum[0]+=e[0]; sum[1]+=e[1]; sum}
p err.map{|e|e/errors.size}
