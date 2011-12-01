#!/usr/bin/env ruby

test_dir = "set-cross"

if __FILE__ == $0
  require 'open3'

  test_dir = ARGV[0] if ARGV[0]

  thresh = 0.5
  thresh = ARGV[1].to_f if ARGV[1]

  def threshold t, result
    result.map{|r| r>=t ? 1 : 0}
  end
end

cross_img_dir = File.join test_dir, "img"
test_list_file = File.join test_dir, "list.txt"


  j_all = []
  errors = []
  matches = 0
  cross_set = File.readlines(test_list_file).map{|l| [l.strip!, l.split("\t")].flatten}
  klasses = cross_set[0].size-2
  err = Array.new(klasses, 0.0)
  true_positive = Array.new(klasses, 0)
  false_positive = Array.new(klasses, 0)
  true_negative = Array.new(klasses, 0)
  false_negative = Array.new(klasses, 0)
  Open3.popen3("./nn_compare -") {|i,o,e,t|
  cross_set.each_with_index do |row, samples|
    f1 = File.join cross_img_dir, "#{row[1]}-1.tif"
    f2 = File.join cross_img_dir, "#{row[1]}-2.tif"
    i << "#{f1} #{f2}\n"
    result = o.gets.to_s.strip
    res = result.split("   ")
    ce = 0.0
    t = threshold(thresh, res[1..-1].map{|r|r.to_f})
    res[1..-1].each_with_index do |e, idx|
      out = row[2+idx].to_i
      log_err = -1*(out* Math.log(e.to_f)) - (1-out)*Math.log(1-e.to_f)
      err[idx] += log_err if log_err==log_err and log_err.abs != Float::INFINITY
      ce += out - t[idx]
      true_positive[idx] += 1 if out==1 and t[idx]==1
      false_positive[idx] += 1 if out==1 and t[idx]==0
      true_negative[idx] += 1 if out==0 and t[idx]==0
      false_negative[idx] += 1 if out==0 and t[idx]==1
    end
    puts "#{row[1]} #{row[2..-1].inspect} #{t.inspect} (#{res[1..-1].join(", ")}) #{err.map{|e| e/(samples+1)}.inspect}"
    matches += 1 if ce==0.0
    errors << err #if err[0] != Float::NAN and err[1] != Float::NAN
  end
  }
  
  err.map!{|e| e / errors.size}
  puts "J_cross = #{err.inspect}"
  j = err.inject(0.0){|sum,e| sum += e}
  puts "J_cross = #{j}"
  puts "matches = %d/%d (%3.2f%%)" % [matches, cross_set.size, matches.to_f*100/cross_set.size]
  p [true_positive, false_positive, true_negative, false_negative]
  klasses.times do |idx|
    precision = true_positive[idx].to_f / (true_positive[idx]+false_positive[idx])
    recall = true_positive[idx].to_f / (true_positive[idx] + false_negative[idx])
    f1score = (precision+recall != 0) ? 2 * (precision * recall) / (precision + recall) : -1
    accuracy = (true_positive[idx] + true_negative[idx]) / (cross_set.size.to_f)
    puts "Class #{idx}"
    puts "Precision: #{precision}"
    puts "Recall: #{recall}"
    puts "F1 Score: #{f1score}"
    puts "Accuracy: #{accuracy}"
  end

