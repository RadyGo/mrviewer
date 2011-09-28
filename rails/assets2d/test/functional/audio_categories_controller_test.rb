require File.dirname(__FILE__) + '/../test_helper'
require 'audio_categories_controller'

# Re-raise errors caught by the controller.
class AudioCategoriesController; def rescue_action(e) raise e end; end

class AudioCategoriesControllerTest < Test::Unit::TestCase
  fixtures :audio_categories

  def setup
    @controller = AudioCategoriesController.new
    @request    = ActionController::TestRequest.new
    @response   = ActionController::TestResponse.new

    @first_id = audio_categories(:first).id
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

    assert_not_nil assigns(:audio_categories)
  end

  def test_show
    get :show, :id => @first_id

    assert_response :success
    assert_template 'show'

    assert_not_nil assigns(:audio_category)
    assert assigns(:audio_category).valid?
  end

  def test_new
    get :new

    assert_response :success
    assert_template 'new'

    assert_not_nil assigns(:audio_category)
  end

  def test_create
    num_audio_categories = AudioCategory.count

    post :create, :audio_category => {}

    assert_response :redirect
    assert_redirected_to :action => 'list'

    assert_equal num_audio_categories + 1, AudioCategory.count
  end

  def test_edit
    get :edit, :id => @first_id

    assert_response :success
    assert_template 'edit'

    assert_not_nil assigns(:audio_category)
    assert assigns(:audio_category).valid?
  end

  def test_update
    post :update, :id => @first_id
    assert_response :redirect
    assert_redirected_to :action => 'show', :id => @first_id
  end

  def test_destroy
    assert_nothing_raised {
      AudioCategory.find(@first_id)
    }

    post :destroy, :id => @first_id
    assert_response :redirect
    assert_redirected_to :action => 'list'

    assert_raise(ActiveRecord::RecordNotFound) {
      AudioCategory.find(@first_id)
    }
  end
end
