require File.dirname(__FILE__) + '/../test_helper'
require 'videos_controller'

# Re-raise errors caught by the controller.
class VideosController; def rescue_action(e) raise e end; end

class VideosControllerTest < Test::Unit::TestCase
  fixtures :videos

  def setup
    @controller = VideosController.new
    @request    = ActionController::TestRequest.new
    @response   = ActionController::TestResponse.new

    @first_id = videos(:first).id
  end

  def test_index
    get :index
    assert_response :success
    assert_template 'list'
  end

  def test_list
    get :list

    assert_response :success
    assert_template 'list'

    assert_not_nil assigns(:videos)
  end

  def test_show
    get :show, :id => @first_id

    assert_response :success
    assert_template 'show'

    assert_not_nil assigns(:video)
    assert assigns(:video).valid?
  end

  def test_new
    get :new

    assert_response :success
    assert_template 'new'

    assert_not_nil assigns(:video)
  end

  def test_create
    num_videos = Video.count

    post :create, :video => {}

    assert_response :redirect
    assert_redirected_to :action => 'list'

    assert_equal num_videos + 1, Video.count
  end

  def test_edit
    get :edit, :id => @first_id

    assert_response :success
    assert_template 'edit'

    assert_not_nil assigns(:video)
    assert assigns(:video).valid?
  end

  def test_update
    post :update, :id => @first_id
    assert_response :redirect
    assert_redirected_to :action => 'show', :id => @first_id
  end

  def test_destroy
    assert_nothing_raised {
      Video.find(@first_id)
    }

    post :destroy, :id => @first_id
    assert_response :redirect
    assert_redirected_to :action => 'list'

    assert_raise(ActiveRecord::RecordNotFound) {
      Video.find(@first_id)
    }
  end
end
