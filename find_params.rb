#!/usr/bin/env ruby

require 'fileutils'
require 'open3'

PARAMS=[30, 10, 3, 1, 0.1, 0.3, 0.03, 0.01, 0.003, 0.001]

train_set = File.readlines("set-train/list.txt").sort_by{rand}[0..12].map{|l| [l.strip!, l.split("\t")].flatten}
img_dir = "set-train/img"

j_params = []

PARAMS.each do |a|
PARAMS.each do |l|
  j_all = []
  cmd = "./nn_learn #{a} #{l} #{img_dir} 200 #{train_set.size}"
  puts cmd
  FileUtils.rm("data.nn") if File.exists?("data.nn")
  Open3.popen3(cmd) {|i,o,e,t|
    i.sync = true
    o.sync = true
    30.times do |c|
      puts ""
      puts "#{c}: #{cmd}"
      errors = []
      train_set.each_with_index do |row,ri|
        i << row[0] << "\n"
        puts o.gets
      end
      i << "update" << "\n"
      puts o.gets
      puts o.gets
      j = o.gets
      puts j
      j =~ /(\S+)$/
      j_all << $1.to_f
    end
    File.open("J_#{a}_#{l}.txt", 'w'){|f| f << j_all.join(",")}
  }
  j_params << [j_all, a, l]
end
end

j_params.sort!{|a,b| a[0][-1] <=> b[0][-1]}
File.open("estimated_params.txt", 'w'){|f| f << j_params[0].inspect }
