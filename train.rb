#!/usr/bin/env ruby

require "open3"

alpha = 0.1
alpha = ARGV[0].to_f if ARGV[0]
lambda = 0.0
lambda = ARGV[1].to_f if ARGV[1]
thresh = 0.5
thresh = ARGV[2].to_f if ARGV[2]
p [alpha, lambda, thresh]
cycles = 1
cycles = ARGV[3].to_i if ARGV[3]

train_dir = "set-train"
train_dir = ARGV[4] if ARGV[4]
img_dir = File.join train_dir, "img"
list_file = File.join train_dir, "list.txt"

test_dir = "set-cross"
test_dir = ARGV[5] if ARGV[5]
cross_img_dir = File.join test_dir, "img"
test_list_file = File.join test_dir, "list.txt"


def threshold t, result
  result.map{|r| r>=t ? 1 : 0}
end

j_all = []

  train_set = File.readlines(list_file).sort_by{rand}.map{|l| [l.strip!, l.split("\t")].flatten}
  cmd = "./nn_learn #{alpha} #{lambda} #{img_dir} 32 #{train_set.size}"
  p cmd

c=0
  Open3.popen3(cmd) {|i,o,e,t|
    i.sync = true
    o.sync = true

while c<cycles do
  puts ""
  puts "Iteration: #{c+1}"
  err = 0.0
  errors = []
    train_set.each_with_index do |row,ri|
      i << row[0] << "\n"
      result = o.gets
      unless result =~ /^f1/
        puts result
        next
      end
      result =~ /(\S+)$/
      err += $1.to_f
      errors << err
      puts "#{c}:#{ri} #{result}"
    end
    c+=1
    puts "updating..."
    i << "update" << "\n"
    puts o.gets
    puts o.gets
    o.gets =~ /(\S+)$/
    err = $1.to_f
    next if err!=err or err==Float::INFINITY # nan or inf
    j_all << err
    File.open('J_train.txt', 'w'){|f| f << j_all.join(",") << "\n"}
    puts "J_train = #{err}"
    break if err < 0.0001 or (j_all.size > 3 and j_all[-1]==j_all[-2] and j_all[-2]==j_all[-3])
    i << "alpha " << (alpha * (j_all.inject(0.0){|x,s| s+=x}/j_all.size)) << "\n"
end
  }
  File.open('J_train.txt', 'w'){|f| f << j_all.join(",")}
  puts "J_train = #{j_all.inspect}"
  puts ""
  
system "./test.rb", test_dir, "#{thresh}"
puts "Iterations: #{c}"
