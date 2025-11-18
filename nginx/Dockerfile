###########
# Stage 1 # Build frontend with Vite
###########
FROM node:20-alpine AS frontend-builder

WORKDIR /app

# Install dependencies
COPY project/frontend/package*.json ./
RUN npm ci

# Copy source and build
COPY project/frontend ./

ARG VITE_API_URL=/api
ENV VITE_API_URL=${VITE_API_URL}

RUN npm run build

###########
# Stage 2 # Nginx reverse proxy + static files
###########
FROM nginx:1.25-alpine

# Install certbot for Let's Encrypt
RUN apk add --no-cache certbot certbot-nginx

# Copy nginx configuration
COPY nginx/nginx.conf /etc/nginx/nginx.conf

# Copy built frontend assets
COPY --from=frontend-builder /app/dist /usr/share/nginx/html

# Create directory for Let's Encrypt challenges
RUN mkdir -p /var/www/certbot

EXPOSE 80 443

CMD ["nginx", "-g", "daemon off;"]
