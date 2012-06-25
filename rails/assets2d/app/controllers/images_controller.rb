class ImagesController < ApplicationController
  active_scaffold :image do |conf|
    conf.columns = [ :thumbnail, :shot, :directory, :filename, 
                     :frame_start, :frame_end,
                     :creator, :width, :height, :pixel_ratio, :date,
                     :format, :fps, :codec, :disk_space, :depth, 
                     :num_channels, :layers, :pixel_format,
                     :fstop, :gamma, 
                     :icc_profile, :render_transform, 
                     :look_mod_transform, :rating, 
                     :online, :backup, :description ]
    conf.actions = [ :list, :show, :update, :search ]
  end
end 
