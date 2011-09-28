module ActiveScaffold::Actions
  module List
    def self.included(base)
      base.before_filter :list_authorized?, :only => [:index, :table, :update_table, :row, :list]
    end

    def index
      list
    end

    def table
      do_list
      render(:action => 'list', :layout => false)
    end

    # This is called when changing pages, sorts and search
    def update_table
      respond_to do |type|
        type.js do
          do_list
          render(:partial => 'list', :layout => false)
        end
        type.html { return_to_main }
      end
    end

    # get just a single row
    def row
      render :partial => 'list_record', :locals => {:record => find_if_allowed(params[:id], :read)}
    end

    def list
      do_list

      respond_to do |type|
        type.html {
          render :action => 'list', :layout => true
        }
        type.xml { render :xml => response_object.to_xml, :content_type => Mime::XML, :status => response_status }
        type.json { render :text => response_object.to_json, :content_type => Mime::JSON, :status => response_status }
        type.yaml { render :text => response_object.to_yaml, :content_type => Mime::YAML, :status => response_status }
      end
    end

    protected

    # The actual algorithm to prepare for the list view
    def do_list
      includes_for_list_columns = active_scaffold_config.list.columns.collect{ |c| c.includes }.flatten.uniq.compact
      self.active_scaffold_joins.concat includes_for_list_columns

      options = {:sorting => active_scaffold_config.list.user.sorting,}
      paginate = (params[:format].nil?) ? (accepts? :html, :js) : [:html, :js].include?(params[:format])
      if paginate
        options.merge!({
          :per_page => active_scaffold_config.list.user.per_page,
          :page => active_scaffold_config.list.user.page
        })
      end

      @page = find_page(options)
      @records = @page.items
    end

    # The default security delegates to ActiveRecordPermissions.
    # You may override the method to customize.
    def list_authorized?
      authorized_for?(:action => :read)
    end
  end
end