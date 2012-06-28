require File.dirname(__FILE__) + '/../test_helper'
require 'icc_profiles_controller'

# Re-raise errors caught by the controller.
class IccProfilesController; def rescue_action(e) raise e end; end

class IccProfilesControllerTest < Test::Unit::TestCase
  fixtures :icc_profiles

  def setup
    @controller = IccProfilesController.new
    @request    = ActionController::TestRequest.new
    @response   = ActionController::TestResponse.new

    @first_id = icc_profiles(:first).id
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

    assert_not_nil assigns(:icc_profiles)
  end

  def test_show
    get :show, :id => @first_id

    assert_response :success
    assert_template 'show'

    assert_not_nil assigns(:icc_profile)
    assert assigns(:icc_profile).valid?
  end

  def test_new
    get :new

    assert_response :success
    assert_template 'new'

    assert_not_nil assigns(:icc_profile)
  end

  def test_create
    num_icc_profiles = IccProfile.count

    post :create, :icc_profile => {}

    assert_response :redirect
    assert_redirected_to :action => 'list'

    assert_equal num_icc_profiles + 1, IccProfile.count
  end

  def test_edit
    get :edit, :id => @first_id

    assert_response :success
    assert_template 'edit'

    assert_not_nil assigns(:icc_profile)
    assert assigns(:icc_profile).valid?
  end

  def test_update
    post :update, :id => @first_id
    assert_response :redirect
    assert_redirected_to :action => 'show', :id => @first_id
  end

  def test_destroy
    assert_nothing_raised {
      IccProfile.find(@first_id)
    }

    post :destroy, :id => @first_id
    assert_response :redirect
    assert_redirected_to :action => 'list'

    assert_raise(ActiveRecord::RecordNotFound) {
      IccProfile.find(@first_id)
    }
  end
end
