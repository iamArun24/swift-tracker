# Render Deployment Guide for SwiftTrack Backend

## Frontend URL
Your frontend is deployed at: **https://swift-tracker.netlify.app**

## Environment Variables for Render

Copy these to your Render dashboard:

### Required Variables:
```
JWT_SECRET=swifttrack_jwt_secret_key_2024_change_this_in_production_very_long_secure_string
ALLOWED_ORIGIN=https://swift-tracker.netlify.app
```

### Database:
- **Add PostgreSQL addon** in Render (Recommended)
- Render will automatically provide: `RENDER_DB_URL`
- The code automatically detects and uses `RENDER_DB_URL` if available

### Port:
- Render automatically provides `PORT` environment variable
- No need to set manually

## Quick Deploy Steps:

1. **Push code to GitHub**
   ```bash
   git add .
   git commit -m "Ready for Render deployment"
   git push origin main
   ```

2. **Create Web Service on Render**
   - Go to render.com
   - Click "New +" → "Web Service"
   - Connect your GitHub repository
   - Configure:
     - **Name**: swifttrack-backend
     - **Region**: Singapore (closest to India)
     - **Branch**: main
     - **Runtime**: Docker
     - **Dockerfile Path**: ./Dockerfile

3. **Add Environment Variables in Render Dashboard:**
   ```
   JWT_SECRET=swifttrack_jwt_secret_key_2024_change_this_in_production_very_long_secure_string
   ALLOWED_ORIGIN=https://swift-tracker.netlify.app
   ```

4. **Add PostgreSQL Database:**
   - In your web service settings, click "Add Database"
   - Select PostgreSQL
   - This will automatically add `RENDER_DB_URL` environment variable

5. **Deploy**
   - Click "Create Web Service"
   - Wait for build and deployment

## Important Notes:

- **Frontend CORS**: Already configured for `https://swift-tracker.netlify.app`
- **Database**: PostgreSQL is automatically used when `RENDER_DB_URL` is present
- **JWT_SECRET**: Change the default to a secure value in production

## Testing After Deployment:

Once deployed, test:
```bash
curl https://your-app.onrender.com/api/health
```

Should return:
```json
{
  "status": "healthy",
  "app": "SwiftTrack",
  "version": "1.0.0",
  "timestamp": 1234567890
}
```
