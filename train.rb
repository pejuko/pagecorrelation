#!/usr/bin/env ruby

require "open3"

alpha = 0.1
alpha = ARGV[0].to_f if ARGV[0]
gamma = 0.0
gamma = ARGV[1].to_f if ARGV[1]
thresh = 0.5
thresh = ARGV[2].to_f if ARGV[2]
p [alpha, gamma, thresh]
cycles = 1
cycles = ARGV[3].to_i if ARGV[3]

train_dir = "set-train"
train_dir = ARGV[4] if ARGV[4]
img_dir = File.join train_dir, "img"
list_file = File.join train_dir, "list.txt"

test_dir = "set-test"
test_dir = ARGV[5] if ARGV[5]
cross_img_dir = File.join test_dir, "img"
test_list_file = File.join test_dir, "list.txt"


def threshold t, result
  result.map{|r| r>=t ? 1 : 0}
end

j_all = []

  train_set = File.readlines(list_file).sort_by{rand}.map{|l| [l.strip!, l.split("\t")].flatten}
  Open3.popen3("./nn_learn #{alpha} #{gamma} #{img_dir}") {|i,o,e,t|
cycles.times do |c|
  puts ""
  puts "Iteration: #{c+1}"
  err = 0.0
  errors = []
    train_set.each do |row|
      i << row[0] << "\n"
      result = o.gets
      result =~ /(\S+)$/
      err += $1.to_f
      errors << err
      puts result
    end
    err = err / errors.size;
    j_all << err
    puts "J_train = #{err}"
end
  }
  puts "J_train = #{j_all.inspect}"
  puts ""
  
  j_all = []
  errors = []
  err = Array.new(train_set[0].size-2, 0.0)
  matches = 0
  cross_set = File.readlines(test_list_file).sort_by{rand}.map{|l| [l.strip!, l.split("\t")].flatten}
  Open3.popen3("./nn_compare -") {|i,o,e,t|
  cross_set.each do |row|
    f1 = File.join cross_img_dir, "#{row[1]}-1.tif"
    f2 = File.join cross_img_dir, "#{row[1]}-2.tif"
    i << "#{f1} #{f2}\n"
    result = o.gets.to_s.strip
    res = result.split("   ")
    ce = 0.0
    t = threshold(thresh, res[1..-1].map{|r|r.to_f})
    res[1..-1].each_with_index do |e, idx|
      err[idx] += -1*(row[2+idx].to_i * Math.log(e.to_f)) - (1-row[2+idx].to_i)*Math.log(1-e.to_f)
      ce += row[2+idx].to_i-t[idx]
    end
    puts "#{row[1]} #{row[2..-1].inspect} #{t.inspect} (#{res[1..-1].join(", ")}) #{err.inspect}"
    matches += 1 if ce==0.0
    errors << err if err[0] != Float::NAN and err[1] != Float::NAN
  end
  }
  
  err.map!{|e| e / errors.size}
  puts "J_cross = #{err.inspect}"
  j = err.inject(0.0){|sum,e| sum += e}
  puts "J_cross = #{j}"
  puts "matches = #{matches}/#{cross_set.size} (#{matches.to_f*100/cross_set.size})"
