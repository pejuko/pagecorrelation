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
  cross_set = File.readlines(test_list_file).sort_by{rand}.map{|l| [l.strip!, l.split("\t")].flatten}
  err = Array.new(cross_set[0].size-2, 0.0)
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
