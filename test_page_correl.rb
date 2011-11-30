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
  matches = 0
  cross_set = File.readlines(test_list_file).map{|l| [l.strip!, l.split("\t")].flatten}
  true_positive = 0
  false_positive = 0
  true_negative = 0
  false_negative = 0
  cross_set.each_with_index do |row, samples|
    f1 = File.join cross_img_dir, "#{row[1]}-1.tif"
    f2 = File.join cross_img_dir, "#{row[1]}-2.tif"
    cmd = "./page_correlation #{f1} #{f2}"
    result = `#{cmd}`
    result =~ /(\d+)$/
    res = $1.to_i
    out = row[2].to_i
    ce = out - res
    true_positive += 1 if out==1 and res==1
    false_positive += 1 if out==0 and res==1
    true_negative += 1 if out==0 and res==0
    false_negative += 1 if out==1 and res==0
    puts "#{row[1]} #{row[2..-1].inspect} (#{res})"
    matches += 1 if ce==0.0
  end
  
  puts "matches = %d/%d (%3.2f%%)" % [matches, cross_set.size, matches.to_f*100/cross_set.size]
  p [true_positive, false_positive, true_negative, false_negative]

  precision = true_positive.to_f / (true_positive+false_positive)
  recall = true_positive.to_f / (true_positive + false_negative)
  f1score = (precision+recall != 0) ? 2 * (precision * recall) / (precision + recall) : -1
  accuracy = (true_positive + true_negative) / (cross_set.size.to_f)
  
  puts "Precision: #{precision}"
  puts "Recall: #{recall}"
  puts "F1 Score: #{f1score}"
  puts "Accuracy: #{accuracy}"

